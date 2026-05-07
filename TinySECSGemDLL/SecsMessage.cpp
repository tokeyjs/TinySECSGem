#include "SecsMessage.h"

#include <cstring>

namespace TinySECSGem
{
    BlockHeader::BlockHeader()
    {
        std::memset(this, 0, sizeof(BlockHeader));
    }

    void BlockHeader::setSessionId(int sessionId)
    {
        m_hiDevId = static_cast<BYTE>((sessionId >> 8) & 0xFF);
        m_loDevId = static_cast<BYTE>(sessionId & 0xFF);
    }

    int BlockHeader::getSessionId() const
    {
        return (static_cast<int>(m_hiDevId) << 8) | static_cast<int>(m_loDevId);
    }

    void BlockHeader::setStreamNum(int stream)
    {
        m_hiMsgId = static_cast<BYTE>(stream & 0x7F);
    }

    unsigned int BlockHeader::getStreamNum() const
    {
        return static_cast<unsigned int>(m_hiMsgId & 0x7F);
    }

    void BlockHeader::setFunctionNum(int function)
    {
        m_loMsgId = static_cast<BYTE>(function & 0xFF);
    }

    unsigned int BlockHeader::getFunctionNum() const
    {
        return static_cast<unsigned int>(m_loMsgId);
    }

    unsigned int BlockHeader::getStreamFunctionNum() const
    {
        return (getStreamNum() << 8) | getFunctionNum();
    }

    void BlockHeader::setPType(int pType)
    {
        m_pType = static_cast<BYTE>(pType & 0xFF);
    }

    int BlockHeader::getPType() const
    {
        return static_cast<int>(m_pType);
    }

    void BlockHeader::setSType(int sType)
    {
        m_sType = static_cast<BYTE>(sType & 0xFF);
    }

    int BlockHeader::getSType() const
    {
        return static_cast<int>(m_sType);
    }

    void BlockHeader::setNeedReply(bool waitBit)
    {
        m_w = waitBit ? 1 : 0;
    }

    bool BlockHeader::isReply() const
    {
        return m_w != 0;
    }

    void BlockHeader::setTransactionNum(uint32_t transactionId)
    {
        m_system[0] = static_cast<BYTE>((transactionId >> 24) & 0xFF);
        m_system[1] = static_cast<BYTE>((transactionId >> 16) & 0xFF);
        m_system[2] = static_cast<BYTE>((transactionId >> 8) & 0xFF);
        m_system[3] = static_cast<BYTE>(transactionId & 0xFF);
    }

    uint32_t BlockHeader::getTransactionNum() const
    {
        return (static_cast<uint32_t>(m_system[0]) << 24) |
               (static_cast<uint32_t>(m_system[1]) << 16) |
               (static_cast<uint32_t>(m_system[2]) << 8) |
               static_cast<uint32_t>(m_system[3]);
    }

    bool BlockHeader::isPrimary() const
    {
        return (getFunctionNum() & 1U) == 1U;
    }

    bool BlockHeader::isSecondary() const
    {
        return !isPrimary();
    }

    bool BlockHeader::isSession() const
    {
        const auto sType = static_cast<EnumHSMS_SType>(getSType());
        return sType != EnumHSMS_SType::STYPE_DATA_MSG;
    }

    bool BlockHeader::wantSessionRsp() const
    {
        const auto sType = static_cast<EnumHSMS_SType>(getSType());
        return sType == EnumHSMS_SType::STYPE_SELECT_REQ ||
               sType == EnumHSMS_SType::STYPE_DESELECT_REQ ||
               sType == EnumHSMS_SType::STYPE_LINKTEST_REQ;
    }

    bool BlockHeader::isExpect(const BlockHeader& other) const
    {
        return getSessionId() == other.getSessionId() &&
               getStreamNum() == other.getStreamNum() &&
               (getFunctionNum() + 1U) == other.getFunctionNum() &&
               getTransactionNum() == other.getTransactionNum() &&
               other.isSecondary();
    }

    void BlockHeader::Set_DataMessage(int sessionId, bool waitBit, int stream, int function, uint32_t transactionId)
    {
        setSessionId(sessionId);
        setNeedReply(waitBit);
        setStreamNum(stream);
        setFunctionNum(function);
        setPType(0);
        setSType(static_cast<int>(EnumHSMS_SType::STYPE_DATA_MSG));
        setTransactionNum(transactionId);
    }

    void BlockHeader::Set_DataMessage_GenSecondaryMsgHeader(const BlockHeader& primaryMsgHeader)
    {
        Set_DataMessage(primaryMsgHeader.getSessionId(),
                        false,
                        primaryMsgHeader.getStreamNum(),
                        primaryMsgHeader.getFunctionNum() + 1,
                        primaryMsgHeader.getTransactionNum());
    }

    void BlockHeader::Set_SelectReq(int sessionId, uint32_t transactionId)
    {
        setSessionId(sessionId);
        setNeedReply(false);
        setStreamNum(0);
        setFunctionNum(0);
        setPType(0);
        setSType(static_cast<int>(EnumHSMS_SType::STYPE_SELECT_REQ));
        setTransactionNum(transactionId);
    }

    void BlockHeader::Set_SelectRsp(int sessionId, int selectStatus, uint32_t transactionId)
    {
        setSessionId(sessionId);
        setNeedReply(false);
        setStreamNum(0);
        setFunctionNum(selectStatus);
        setPType(0);
        setSType(static_cast<int>(EnumHSMS_SType::STYPE_SELECT_RSP));
        setTransactionNum(transactionId);
    }

    void BlockHeader::Set_DeselectReq(int sessionId, uint32_t transactionId)
    {
        setSessionId(sessionId);
        setNeedReply(false);
        setStreamNum(0);
        setFunctionNum(0);
        setPType(0);
        setSType(static_cast<int>(EnumHSMS_SType::STYPE_DESELECT_REQ));
        setTransactionNum(transactionId);
    }

    void BlockHeader::Set_DeselectRsp(int sessionId, int deselectStatus, uint32_t transactionId)
    {
        setSessionId(sessionId);
        setNeedReply(false);
        setStreamNum(0);
        setFunctionNum(deselectStatus);
        setPType(0);
        setSType(static_cast<int>(EnumHSMS_SType::STYPE_DESELECT_RSP));
        setTransactionNum(transactionId);
    }

    void BlockHeader::Set_LinktestReq(uint32_t transactionId)
    {
        setSessionId(0xFFFF);
        setNeedReply(false);
        setStreamNum(0);
        setFunctionNum(0);
        setPType(0);
        setSType(static_cast<int>(EnumHSMS_SType::STYPE_LINKTEST_REQ));
        setTransactionNum(transactionId);
    }

    void BlockHeader::Set_LinktestRsp(uint32_t transactionId)
    {
        setSessionId(0xFFFF);
        setNeedReply(false);
        setStreamNum(0);
        setFunctionNum(0);
        setPType(0);
        setSType(static_cast<int>(EnumHSMS_SType::STYPE_LINKTEST_RSP));
        setTransactionNum(transactionId);
    }

    void BlockHeader::Set_RejectReq(int sessionId, int byte2, int reasonCode, uint32_t transactionId)
    {
        setSessionId(sessionId);
        setNeedReply(false);
        setStreamNum(byte2);
        setFunctionNum(reasonCode);
        setPType(0);
        setSType(static_cast<int>(EnumHSMS_SType::STYPE_REJECT_REQ));
        setTransactionNum(transactionId);
    }

    void BlockHeader::Set_SeparateReq(int sessionId, uint32_t transactionId)
    {
        setSessionId(sessionId);
        setNeedReply(false);
        setStreamNum(0);
        setFunctionNum(0);
        setPType(0);
        setSType(static_cast<int>(EnumHSMS_SType::STYPE_SEPARATE_REQ));
        setTransactionNum(transactionId);
    }

    SECSMessage::SECSMessage() = default;

    void SECSMessage::SetHead(const BlockHeader& head)
    {
        m_head = head;
    }

    void SECSMessage::SetHead(const BlockHeader* head)
    {
        if (head != nullptr)
        {
            m_head = *head;
        }
    }

    BlockHeader& SECSMessage::GetHead()
    {
        return m_head;
    }

    const BlockHeader& SECSMessage::GetHead() const
    {
        return m_head;
    }

    BlockHeader* SECSMessage::GetHeadPtr()
    {
        return &m_head;
    }

    std::shared_ptr<BaseItem> SECSMessage::GetData() const
    {
        return m_data;
    }

    void SECSMessage::SetData(std::shared_ptr<BaseItem> data)
    {
        m_data = std::move(data);
    }

    void SECSMessage::SetData(BaseItem* data)
    {
        m_data.reset(data);
    }

    bool SECSMessage::isItemEmpty() const
    {
        return m_data == nullptr;
    }

    bool SECSMessage::isDataMessage() const
    {
        return !m_head.isSession();
    }

    bool SECSMessage::isNeedReply() const
    {
        return m_head.isReply();
    }

    int SECSMessage::GetStream() const
    {
        return static_cast<int>(m_head.getStreamNum());
    }

    int SECSMessage::GetFunction() const
    {
        return static_cast<int>(m_head.getFunctionNum());
    }

    int SECSMessage::GetSessionID() const
    {
        return m_head.getSessionId();
    }

    uint32_t SECSMessage::GetTransactionID() const
    {
        return m_head.getTransactionNum();
    }

    size_t SECSMessage::SerializationMsg(BYTE* buffer, size_t bufferSize) const
    {
        if (buffer == nullptr || bufferSize < sizeof(BlockHeader))
        {
            return 0;
        }

        size_t index = 0;
        std::memcpy(buffer, &m_head, sizeof(BlockHeader));
        index += sizeof(BlockHeader);

        if (m_data == nullptr)
        {
            return index;
        }

        const int itemSize = m_data->Serialization(buffer + index, bufferSize - index);
        if (itemSize <= 0)
        {
            return 0;
        }

        index += static_cast<size_t>(itemSize);
        return index;
    }

    std::vector<uint8_t> SECSMessage::SerializeForWire() const
    {
        const size_t payloadCapacity = sizeof(BlockHeader) + (m_data ? m_data->totalByteSize() : 0U);
        std::vector<uint8_t> payload(payloadCapacity);
        const size_t payloadLength = SerializationMsg(payload.data(), payload.size());
        if (payloadLength == 0)
        {
            return {};
        }

        payload.resize(payloadLength + 4U);
        std::memmove(payload.data() + 4, payload.data(), payloadLength);

        const uint32_t bodyLength = static_cast<uint32_t>(payloadLength);
        payload[0] = static_cast<uint8_t>((bodyLength >> 24) & 0xFFU);
        payload[1] = static_cast<uint8_t>((bodyLength >> 16) & 0xFFU);
        payload[2] = static_cast<uint8_t>((bodyLength >> 8) & 0xFFU);
        payload[3] = static_cast<uint8_t>(bodyLength & 0xFFU);
        return payload;
    }

    std::shared_ptr<SECSMessage> SecsMessageParser::ParsePayload(const BYTE* data, size_t length, bool& ok)
    {
        ok = false;
        if (data == nullptr || length < sizeof(BlockHeader))
        {
            return {};
        }

        auto message = std::make_shared<SECSMessage>();
        BlockHeader header = ParseHeader(data, length, ok);
        if (!ok)
        {
            return nullptr;
        }
        message->SetHead(header);

        if (length == sizeof(BlockHeader))
        {
            ok = true;
            return message;
        }

        bool itemOk = true;
        BaseItem* item = BaseItem::Deserialization(const_cast<BYTE*>(data) + sizeof(BlockHeader),
                                                   const_cast<BYTE*>(data) + length,
                                                   itemOk);
        if (!itemOk)
        {
            return {};
        }

        message->SetData(std::shared_ptr<BaseItem>(item));
        ok = true;
        return message;
    }

    BlockHeader SecsMessageParser::ParseHeader(const BYTE* data, size_t length, bool& ok)
    {
        BlockHeader header;
        ok = false;
        if (data == nullptr || length < sizeof(BlockHeader))
        {
            return header;
        }
        std::memcpy(&header, data, sizeof(BlockHeader));
        ok = true;
        return header;
    }

    std::shared_ptr<SECSMessage> SecsMessageParser::ParseWireMessage(const BYTE* data, size_t length, bool& ok)
    {
        ok = false;
        if (data == nullptr || length < 4)
        {
            return {};
        }

        const uint32_t payloadLength = (static_cast<uint32_t>(data[0]) << 24) |
                                       (static_cast<uint32_t>(data[1]) << 16) |
                                       (static_cast<uint32_t>(data[2]) << 8) |
                                       static_cast<uint32_t>(data[3]);
        if (payloadLength + 4U != length)
        {
            return {};
        }

        return ParsePayload(data + 4, payloadLength, ok);
    }
}
