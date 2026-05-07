#include "SecsGemLibrary.h"
#include "HPSocketTransport.h"
#include "Stream9Message.h"
#include "Internal.h"
#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>


std::string BytesToHexPretty(const BYTE* data, size_t len)
{
    static const char hex[] = "0123456789ABCDEF";
    std::string result;
    result.reserve(len * 3);
    for (size_t i = 0; i < len; ++i)
    {
        BYTE b = data[i];
        result.push_back(hex[b >> 4]);
        result.push_back(hex[b & 0xF]);
        result.push_back(' ');
    }
    if (!result.empty())
        result.pop_back(); // 去掉最后空格
    return result;
}


namespace TinySECSGem
{
    namespace
    {
        class DefaultSECSMsgValidator final : public ISECSMsgValidator
        {
        };

        ISECSMsgValidator& GetDefaultValidator()
        {
            static DefaultSECSMsgValidator s_defaultValidator;
            return s_defaultValidator;
        }

        // Outstanding transactions are tracked differently for data and control flows
        // because E37 uses T3 for data reply and T6 for control reply.
        enum class PendingKind
        {
            DataMessage,
            Select,
            Deselect,
            Linktest,
        };

        struct PendingTransaction
        {
            PendingKind kind;
            std::chrono::steady_clock::time_point deadline;
            BlockHeader header;
        };
    }

    class SecsGem::Impl final : public HPSocketTransport::Listener
    {
    public:
        Impl()
            : m_transport(*this),
            m_handler(nullptr),
            m_msgValidator(&GetDefaultValidator()),
            m_state(HsmsConnectionState::NotConnected),
            m_running(false),
            m_stopRequested(false),
            m_pendingPullBytes(0),
            m_partialFrameSince(),
            m_connectedSince(),
            m_nextReconnect(),
            m_nextSystemBytes(1)
        {
        }

        ~Impl() override
        {
            Stop();
        }

        void SetParameter(const SocketParameter& parameter)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_parameter = parameter;
        }

        SocketParameter GetParameter() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_parameter;
        }

        void SetHandler(ISecsGemHandler* handler)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_handler = handler;
        }

        void SetSECSMsgValidator(ISECSMsgValidator* validator)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_msgValidator = validator ? validator : &GetDefaultValidator();
        }

        bool Start()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_running)
            {
                return true;
            }

            m_stopRequested = false;
            m_pendingPullBytes = 0;
            m_pending.clear();
            m_state = HsmsConnectionState::NotConnected;
            m_nextReconnect = std::chrono::steady_clock::now();

            // Starting transport only establishes TCP; HSMS SELECT is handled later.
            if (!m_transport.Start(m_parameter))
            {
                return false;
            }

            m_running = true;
            m_timerThread = std::thread(&Impl::TimerLoop, this);
            return true;
        }

        void Stop()
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (!m_running && !m_timerThread.joinable())
                {
                    return;
                }
                m_stopRequested = true;
            }

            m_transport.Stop();

            if (m_timerThread.joinable())
            {
                m_timerThread.join();
            }

            std::lock_guard<std::mutex> lock(m_mutex);
            m_pendingPullBytes = 0;
            m_pending.clear();
            m_state = HsmsConnectionState::NotConnected;
            m_running = false;
        }

        bool SendSECSMessage(const SECSMessage& message)
        {
            auto wire = message.SerializeForWire();
            if (wire.empty())
            {
                return false;
            }

            const bool sent = m_transport.Send(wire.data(), wire.size());
            if (!sent)
            {
                return false;
            }

            std::string strLog = BytesToHexPretty(wire.data(), wire.size());
            SECSLOG_DEBUG("Send(SECS-I):%s\n", strLog.c_str());

            if (message.isDataMessage())
                SECSLOG_DEBUG("Send(SECS-II): S%dF%d TransactionID=%d\n%s\n", message.GetStream(), message.GetFunction(), message.GetTransactionID(), message.GetData() ? message.GetData()->print().c_str() : "");


            // Track transactions only after bytes are accepted by the transport layer.
            TrackIfNeeded(message);
            return true;
        }

        bool SendDataMessage(uint8_t stream, uint8_t function, bool waitReply, std::shared_ptr<BaseItem> data)
        {
            SECSMessage message;
            BlockHeader header;
            header.Set_DataMessage(m_parameter.m_deviceId, waitReply, stream, function, NextSystemBytes());
            message.SetHead(header);
            message.SetData(std::move(data));
            return SendSECSMessage(message);
        }

        bool SendSelectRequest()
        {
            SECSMessage message;
            BlockHeader header;
            header.Set_SelectReq(m_parameter.m_deviceId, NextSystemBytes());
            message.SetHead(header);
            return SendSECSMessage(message);
        }

        bool SendDeselectRequest()
        {
            SECSMessage message;
            BlockHeader header;
            header.Set_DeselectReq(m_parameter.m_deviceId, NextSystemBytes());
            message.SetHead(header);
            return SendSECSMessage(message);
        }

        bool SendLinktestRequest()
        {
            SECSMessage message;
            BlockHeader header;
            header.Set_LinktestReq(NextSystemBytes());
            message.SetHead(header);
            return SendSECSMessage(message);
        }

        bool SendSeparateRequest()
        {
            SECSMessage message;
            BlockHeader header;
            header.Set_SeparateReq(m_parameter.m_deviceId, NextSystemBytes());
            message.SetHead(header);
            return SendSECSMessage(message);
        }

        HsmsConnectionState GetConnectionState() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_state;
        }

        bool IsRunning() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_running;
        }

        bool IsSelected() const
        {
            return GetConnectionState() == HsmsConnectionState::Selected;
        }

        void OnTransportConnected() override
        {
            ISecsGemHandler* handler = nullptr;
            bool autoSelect = false;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_state = HsmsConnectionState::ConnectedNotSelected;
                m_connectedSince = std::chrono::steady_clock::now();
                m_partialFrameSince = {};
                m_pendingPullBytes = 0;
                handler = m_handler;
                // Active mode usually initiates SELECT automatically after TCP connect.
                autoSelect = m_parameter.m_autoSelect && m_parameter.m_mode == EnumHSMSMode::EnumHSMSMode_Active;
            }

            if (handler != nullptr)
            {
                handler->OnConnected();
            }

            if (autoSelect)
            {
                SendSelectRequest();
            }
        }

        void OnTransportDisconnected() override
        {
            ISecsGemHandler* handler = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_state = HsmsConnectionState::NotConnected;
                m_pending.clear();
                m_pendingPullBytes = 0;
                m_partialFrameSince = {};
                m_connectedSince = {};
                handler = m_handler;
                if (m_parameter.m_mode == EnumHSMSMode::EnumHSMSMode_Active && m_parameter.m_autoReconnect && !m_stopRequested)
                {
                    m_nextReconnect = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_parameter.m_t5timeout);
                }
            }

            if (handler != nullptr)
            {
                handler->OnDisconnected();
            }
        }

        void OnTransportReadable(size_t length) override
        {
            if (length == 0)
            {
                return;
            }

            std::vector<std::shared_ptr<SECSMessage>> messages;
            bool protocolError = false;
            bool protocolError_DataTooLong = false;
            uint32_t payloadLengthError = 0;
            BlockHeader errorHeader;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_pendingPullBytes += length;
                if (m_pendingPullBytes > 0)
                {
                    m_partialFrameSince = std::chrono::steady_clock::now();
                }

                // Pull mode keeps the unread bytes inside HP-Socket; parse one full HSMS frame at a time.
                while (m_pendingPullBytes >= 4)
                {
                    uint8_t lengthBytes[4]{};
                    if (!m_transport.Peek(lengthBytes, sizeof(lengthBytes)))
                    {
                        protocolError = true;
                        break;
                    }

                    const uint32_t payloadLength = (static_cast<uint32_t>(lengthBytes[0]) << 24) |
                        (static_cast<uint32_t>(lengthBytes[1]) << 16) |
                        (static_cast<uint32_t>(lengthBytes[2]) << 8) |
                        static_cast<uint32_t>(lengthBytes[3]);
                    if (payloadLength > HSMSRECV_BUFFER_SIZE)
                    {
                        protocolError_DataTooLong = true;
                        payloadLengthError = payloadLength;
                        if (payloadLengthError <= HSMSRECV_BUFFER_MAXSIZE)
                        {
                            if (m_pendingPullBytes < payloadLengthError + 4U)
                            {
                                break;
                            }
                            else
                            {
                                std::vector<uint8_t> frame(payloadLengthError + 4U);
                                if (!m_transport.Fetch(frame.data(), frame.size()))
                                {
                                    protocolError = true;
                                    break;
                                }
                                m_pendingPullBytes -= frame.size();
                                bool bOk = false;
                                errorHeader = SecsMessageParser::ParseHeader(frame.data() + 4U, sizeof(BlockHeader), bOk);
                                if (!bOk)
                                {
                                    protocolError = true;
                                    break;
                                }
                            }
                        }
                        break;
                    }

                    if (m_pendingPullBytes < payloadLength + 4U)
                    {
                        break;
                    }

                    std::vector<uint8_t> frame(payloadLength + 4U);
                    if (!m_transport.Fetch(frame.data(), frame.size()))
                    {
                        protocolError = true;
                        break;
                    }
                    m_pendingPullBytes -= frame.size();

                    std::string strLog = BytesToHexPretty(frame.data(), frame.size());
                    SECSLOG_DEBUG("Recv(SECS-I):%s\n", strLog.c_str());

                    bool ok = false;
                    auto message = SecsMessageParser::ParseWireMessage(frame.data(), frame.size(), ok);
                    if (!ok || !message)
                    {
                        protocolError = true;
                        break;
                    }
                    messages.push_back(std::move(message));
                }

                if (m_pendingPullBytes == 0)
                {
                    m_partialFrameSince = {};
                }
            }

            if (protocolError_DataTooLong)
            {
                if (payloadLengthError > HSMSRECV_BUFFER_MAXSIZE)
                {
                    ReportProtocolError("Invalid HSMS frame. Data Too Long, disconnect!!!!");
                    m_transport.Disconnect();
                    return;
                }
                else 
                {
                    ReportProtocolError("Invalid HSMS frame. Data Too Long, Send S9F11 !");
                    SECSMessage reply = Stream9Message::GenerateS9F11Message(errorHeader);
                    SendSECSMessage(reply);
                    return;
                }
            }

            if (protocolError)
            {
                ReportProtocolError("Invalid HSMS frame.");
                m_transport.Disconnect();
                return;
            }

            for (const auto& message : messages)
            {
                HandleMessage(*message);
            }
        }

        void OnTransportError(const char* message) override
        {
            ReportProtocolError(message);
        }

    private:
        void TimerLoop()
        {
            while (true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                bool stop = false;
                bool shouldReconnect = false;
                bool shouldDisconnect = false;
                std::vector<PendingTransaction> T3TimeoutMsgVec;
                std::vector<std::pair<std::string, uint32_t>> expired;
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    stop = m_stopRequested;
                    if (stop)
                    {
                        break;
                    }

                    const auto now = std::chrono::steady_clock::now();

                    // T5: reconnect delay for active mode after a disconnect.
                    if (m_parameter.m_mode == EnumHSMSMode::EnumHSMSMode_Active &&
                        m_parameter.m_autoReconnect &&
                        !m_transport.IsStarted() &&
                        now >= m_nextReconnect)
                    {
                        shouldReconnect = true;
                        m_nextReconnect = now + std::chrono::milliseconds(m_parameter.m_t5timeout);
                    }

                    // T7: after TCP connected, SELECT must complete within the configured window.
                    if (m_state == HsmsConnectionState::ConnectedNotSelected &&
                        m_connectedSince.time_since_epoch().count() > 0 &&
                        now - m_connectedSince >= std::chrono::milliseconds(m_parameter.m_t7timeout))
                    {
                        expired.emplace_back("T7", 0);
                        shouldDisconnect = true;
                    }

                    // T8: incomplete frame stayed too long in receive buffer.
                    if (m_pendingPullBytes > 0 &&
                        m_partialFrameSince.time_since_epoch().count() > 0 &&
                        now - m_partialFrameSince >= std::chrono::milliseconds(m_parameter.m_t8timeout))
                    {
                        expired.emplace_back("T8", 0);
                        shouldDisconnect = true;
                    }

                    // T3/T6: outstanding transactions expire independently.
                    for (auto it = m_pending.begin(); it != m_pending.end();)
                    {
                        if (now >= it->second.deadline)
                        {
                            const char* timerName = it->second.kind == PendingKind::DataMessage ? "T3" : "T6";
                            if (it->second.kind == PendingKind::DataMessage)
                                T3TimeoutMsgVec.push_back(it->second);
                            expired.emplace_back(timerName, it->first);
                            it = m_pending.erase(it);
                        }
                        else
                        {
                            ++it;
                        }
                    }
                }

                for (int i = 0; i < T3TimeoutMsgVec.size(); i++)
                {
                    SECSMessage reply = Stream9Message::GenerateS9F9Message(T3TimeoutMsgVec[i].header);
                    SendSECSMessage(reply);
                }

                for (const auto& item : expired)
                {
                    ReportTimeout(item.first.c_str(), item.second);
                }

                if (shouldDisconnect)
                {
                    m_transport.Disconnect();
                }

                if (shouldReconnect)
                {
                    m_transport.Start(m_parameter);
                }
            }
        }

        bool ValidateAndCloseDataReply(const SECSMessage& message)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            auto it = m_pending.find(message.GetTransactionID());
            if (it == m_pending.end())
            {
                return false;
            }

            const PendingTransaction& pending = it->second;
            if (pending.kind != PendingKind::DataMessage)
            {
                return false;
            }

            if (!pending.header.isExpect(message.GetHead()))
            {
                return false;
            }

            m_pending.erase(it);
            return true;
        }

        bool CloseExpectedControlReply(const BlockHeader& header, PendingKind expectedKind)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_pending.find(header.getTransactionNum());
            if (it == m_pending.end() || it->second.kind != expectedKind)
            {
                return false;
            }

            m_pending.erase(it);
            return true;
        }

        void HandleMessage(const SECSMessage& message)
        {
            if (message.GetHead().getPType() != 0)
            {
                SendReject(message.GetHead(), EnumHSMS_RejectStatus::HSMS_NOT_SUPPORT_PTYPE);
                return;
            }

            // E37 separates control messages (SType != 0) from SECS-II data messages.
            if (message.isDataMessage())
            {
                SECSLOG_DEBUG("Recv(SECS-II): S%dF%d TransactionID=%d\n%s\n", message.GetStream(), message.GetFunction(), message.GetTransactionID(), message.GetData() ? message.GetData()->print().c_str() : "");

                HandleDataMessage(message);
                return;
            }

            HandleControlMessage(message.GetHead());
        }

        void HandleDataMessage(const SECSMessage& message)
        {
            ISecsGemHandler* handler = nullptr;
            ISECSMsgValidator* validator = nullptr;
            int deviceId = 0;
            bool selected = false;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                selected = m_state == HsmsConnectionState::Selected;
                handler = m_handler;
                validator = m_msgValidator;
                deviceId = m_parameter.m_deviceId;
            }

            // Data messages are valid only after HSMS SELECT has completed.
            if (!selected)
            {
                SendReject(message.GetHead(), EnumHSMS_RejectStatus::HSMS_NOT_SELECTED_ENTITY);
                return;
            }

            if (message.GetSessionID() != deviceId)
            {
                // S9F1
                SECSMessage reply = Stream9Message::GenerateS9F1Message(message.GetHead());
                SendSECSMessage(reply);
                return;
            }

            const bool isSecondary = message.GetHead().isSecondary();
            if (isSecondary)
            {
                if (!ValidateAndCloseDataReply(message))
                {
                    SendReject(message.GetHead(), EnumHSMS_RejectStatus::HSMS_NOT_OPEN_TRANSACTION);
                    return;
                }
            }


            if (!validator->CheckStream(message))
            {
                // S9F3
                SECSMessage reply = Stream9Message::GenerateS9F3Message(message.GetHead());
                SendSECSMessage(reply);
                return;
            }

            if (!validator->CheckFunction(message))
            {
                // S9F5
                SECSMessage reply = Stream9Message::GenerateS9F5Message(message.GetHead());
                SendSECSMessage(reply);
                return;
            }

            if (!validator->CheckMessageData(message))
            {
                // S9F7
                SECSMessage reply = Stream9Message::GenerateS9F7Message(message.GetHead());
                SendSECSMessage(reply);
                return;
            }

            if (message.GetHead().isPrimary())
            {
                if (handler != nullptr)
                {
                    handler->OnPrimaryMessage(message);
                }
            }
            else
            {
                if (handler != nullptr)
                {
                    handler->OnSecondaryMessage(message);
                }
            }
        }

        void HandleControlMessage(const BlockHeader& header)
        {
            ISecsGemHandler* handler = nullptr;
            bool notifySelected = false;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                handler = m_handler;
            }

            switch (static_cast<EnumHSMS_SType>(header.getSType()))
            {
            case EnumHSMS_SType::STYPE_SELECT_REQ:
            {
                EnumHSMS_SelectStatus stat;
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    if (m_state != HsmsConnectionState::Selected)
                    {
                        // On passive side, accepting SELECT moves the session into selected state.
                        m_state = HsmsConnectionState::Selected;
                        notifySelected = true;
                        stat = EnumHSMS_SelectStatus::HSMS_SELECT_OK;
                    }
                    else
                    {
                        stat = EnumHSMS_SelectStatus::HSMS_SELECT_ACTIVE;
                    }
                }
                SendSelectResponse(header, stat);
            }
            break;
            case EnumHSMS_SType::STYPE_SELECT_RSP:
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_pending.erase(header.getTransactionNum());
                // SELECT response function byte carries the status code.
                if (header.getFunctionNum() == static_cast<uint8_t>(EnumHSMS_SelectStatus::HSMS_SELECT_OK))
                {
                    m_state = HsmsConnectionState::Selected;
                    notifySelected = true;
                }

                /// Stricter verification, not to be used for now.
                //if (!CloseExpectedControlReply(header, PendingKind::Select))
                //{
                //    SendReject(header, EnumHSMS_RejectStatus::HSMS_NOT_OPEN_TRANSACTION);
                //}
                //else
                //{
                //    std::lock_guard<std::mutex> lock(m_mutex);
                //    // SELECT response function byte carries the status code.
                //    if (header.getFunctionNum() == static_cast<uint8_t>(EnumHSMS_SelectStatus::HSMS_SELECT_OK))
                //    {
                //        m_state = HsmsConnectionState::Selected;
                //        notifySelected = true;
                //    }
                //}
            }
            break;
            case EnumHSMS_SType::STYPE_DESELECT_REQ:
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_state = HsmsConnectionState::ConnectedNotSelected;
            }
            SendDeselectResponse(header, EnumHSMS_DeselectStatus::HSMS_DESELECT_OK);
            break;
            case EnumHSMS_SType::STYPE_DESELECT_RSP:
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_pending.erase(header.getTransactionNum());
                m_state = HsmsConnectionState::ConnectedNotSelected;
            }
            break;
            case EnumHSMS_SType::STYPE_LINKTEST_REQ:
                SendLinktestResponse(header);
                break;
            case EnumHSMS_SType::STYPE_LINKTEST_RSP:
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_pending.erase(header.getTransactionNum());
            }
            break;
            case EnumHSMS_SType::STYPE_REJECT_REQ:
                break;
            case EnumHSMS_SType::STYPE_SEPARATE_REQ:
                // Separate means the peer is intentionally tearing the session down.
                m_transport.Disconnect();
                break;
            default:
                SendReject(header, EnumHSMS_RejectStatus::HSMS_NOT_SUPPORT_STYPE);
                break;
            }

            if (handler != nullptr)
            {
                handler->OnControlMessage(header);
                if (notifySelected)
                {
                    handler->OnSelected();
                }
            }
        }

        void SendSelectResponse(const BlockHeader& request, EnumHSMS_SelectStatus status)
        {
            SECSMessage message;
            BlockHeader header;
            header.Set_SelectRsp(request.getSessionId(), static_cast<int>(status), request.getTransactionNum());
            message.SetHead(header);
            SendSECSMessage(message);
        }

        void SendDeselectResponse(const BlockHeader& request, EnumHSMS_DeselectStatus status)
        {
            SECSMessage message;
            BlockHeader header;
            header.Set_DeselectRsp(request.getSessionId(), static_cast<int>(status), request.getTransactionNum());
            message.SetHead(header);
            SendSECSMessage(message);
        }

        void SendLinktestResponse(const BlockHeader& request)
        {
            SECSMessage message;
            BlockHeader header;
            header.Set_LinktestRsp(request.getTransactionNum());
            message.SetHead(header);
            SendSECSMessage(message);
        }

        void SendReject(const BlockHeader& request, EnumHSMS_RejectStatus reason)
        {
            SECSMessage message;
            BlockHeader header;
            const int byte2 = request.getPType() != 0 ? request.getPType() : request.getSType();
            header.Set_RejectReq(request.getSessionId(), byte2, static_cast<int>(reason), request.getTransactionNum());
            message.SetHead(header);
            SendSECSMessage(message);
        }

        void TrackIfNeeded(const SECSMessage& message)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            const auto now = std::chrono::steady_clock::now();

            PendingTransaction transaction{};
            bool track = false;
            // Wait-bit primary data message expects a secondary reply under T3.
            if (message.isDataMessage() && message.isNeedReply())
            {
                transaction.kind = PendingKind::DataMessage;
                transaction.deadline = now + std::chrono::milliseconds(m_parameter.m_t3timeout);
                transaction.header = message.GetHead();
                track = true;
            }
            // Select/Deselect/Linktest are session control exchanges and use T6.
            else if (!message.isDataMessage() && message.GetHead().wantSessionRsp())
            {
                switch (static_cast<EnumHSMS_SType>(message.GetHead().getSType()))
                {
                case EnumHSMS_SType::STYPE_SELECT_REQ:
                    transaction.kind = PendingKind::Select;
                    break;
                case EnumHSMS_SType::STYPE_DESELECT_REQ:
                    transaction.kind = PendingKind::Deselect;
                    break;
                case EnumHSMS_SType::STYPE_LINKTEST_REQ:
                    transaction.kind = PendingKind::Linktest;
                    break;
                default:
                    break;
                }
                transaction.deadline = now + std::chrono::milliseconds(m_parameter.m_t6timeout);
                transaction.header = message.GetHead();
                track = true;
            }

            if (track)
            {
                m_pending[message.GetTransactionID()] = transaction;
            }
        }

        uint32_t NextSystemBytes()
        {
            return m_nextSystemBytes.fetch_add(1);
        }

        void ReportProtocolError(const char* message)
        {
            ISecsGemHandler* handler = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                handler = m_handler;
            }

            if (handler != nullptr)
            {
                handler->OnProtocolError(message);
            }
        }

        void ReportTimeout(const char* timerName, uint32_t transactionId)
        {
            ISecsGemHandler* handler = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                handler = m_handler;
            }
            if (handler != nullptr)
            {
                handler->OnTimeout(timerName, transactionId);
            }
        }

        mutable std::mutex m_mutex;
        HPSocketTransport m_transport;
        SocketParameter m_parameter;
        ISecsGemHandler* m_handler;
        ISECSMsgValidator* m_msgValidator;
        HsmsConnectionState m_state;
        bool m_running;
        bool m_stopRequested;
        // Bytes currently buffered inside HP-Socket pull mode and not fetched yet.
        size_t m_pendingPullBytes;
        std::unordered_map<uint32_t, PendingTransaction> m_pending;
        std::chrono::steady_clock::time_point m_partialFrameSince;
        std::chrono::steady_clock::time_point m_connectedSince;
        std::chrono::steady_clock::time_point m_nextReconnect;
        std::atomic<uint32_t> m_nextSystemBytes;
        std::thread m_timerThread;
    };

    SecsGem::SecsGem()
        : m_impl(std::unique_ptr<Impl>(new Impl()))
    {
    }

    SecsGem::~SecsGem() = default;

    const char* SecsGem::GetVersion()
    {
        return TINYSECSGEM_VERSION_STRING;
    }

    void SecsGem::SetParameter(const SocketParameter& parameter)
    {
        m_impl->SetParameter(parameter);
    }

    SocketParameter SecsGem::GetParameter() const
    {
        return m_impl->GetParameter();
    }

    void SecsGem::SetHandler(ISecsGemHandler* handler)
    {
        m_impl->SetHandler(handler);
    }

    void SecsGem::SetSECSMsgValidator(ISECSMsgValidator* validator)
    {
        m_impl->SetSECSMsgValidator(validator);
    }

    bool SecsGem::Start()
    {
        return m_impl->Start();
    }

    void SecsGem::Stop()
    {
        m_impl->Stop();
    }

    bool SecsGem::SendSECSMessage(const SECSMessage& message)
    {
        return m_impl->SendSECSMessage(message);
    }

    bool SecsGem::SendDataMessage(uint8_t stream, uint8_t function, bool waitReply, std::shared_ptr<BaseItem> data)
    {
        return m_impl->SendDataMessage(stream, function, waitReply, std::move(data));
    }

    bool SecsGem::SendSelectRequest()
    {
        return m_impl->SendSelectRequest();
    }

    bool SecsGem::SendDeselectRequest()
    {
        return m_impl->SendDeselectRequest();
    }

    bool SecsGem::SendLinktestRequest()
    {
        return m_impl->SendLinktestRequest();
    }

    bool SecsGem::SendSeparateRequest()
    {
        return m_impl->SendSeparateRequest();
    }

    HsmsConnectionState SecsGem::GetConnectionState() const
    {
        return m_impl->GetConnectionState();
    }

    bool SecsGem::IsRunning() const
    {
        return m_impl->IsRunning();
    }

    bool SecsGem::IsSelected() const
    {
        return m_impl->IsSelected();
    }
}
