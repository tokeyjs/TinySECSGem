#include "HPSocketTransport.h"
#include <climits>
#include <string>
#include <vector>

namespace TinySECSGem
{
    class HPSocketTransport::ServerListenerImpl : public CTcpPullServerListener
    {
    public:
        explicit ServerListenerImpl(HPSocketTransport& owner)
            : m_owner(owner)
        {
        }

        EnHandleResult OnPrepareListen(ITcpServer*, SOCKET) override { return HR_OK; }
        EnHandleResult OnAccept(ITcpServer*, CONNID dwConnID, UINT_PTR) override
        {
            m_owner.HandleConnected(dwConnID);
            return HR_OK;
        }
        EnHandleResult OnHandShake(ITcpServer*, CONNID) override { return HR_OK; }
        EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength) override
        {
            // Pull mode gives only byte count here; bytes remain buffered inside HP-Socket.
            return m_owner.HandleServerReceive(pSender, dwConnID, iLength) ? HR_OK : HR_ERROR;
        }
        EnHandleResult OnSend(ITcpServer*, CONNID, const BYTE*, int) override { return HR_OK; }
        EnHandleResult OnClose(ITcpServer*, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
        {
            m_owner.HandleDisconnected(dwConnID, enOperation, iErrorCode);
            return HR_OK;
        }
        EnHandleResult OnShutdown(ITcpServer*) override
        {
            return HR_OK;
        }

    private:
        HPSocketTransport& m_owner;
    };

    class HPSocketTransport::ClientListenerImpl : public CTcpPullClientListener
    {
    public:
        explicit ClientListenerImpl(HPSocketTransport& owner)
            : m_owner(owner)
        {
        }

        EnHandleResult OnPrepareConnect(ITcpClient*, CONNID, SOCKET) override { return HR_OK; }
        EnHandleResult OnConnect(ITcpClient*, CONNID dwConnID) override
        {
            m_owner.HandleConnected(dwConnID);
            return HR_OK;
        }
        EnHandleResult OnHandShake(ITcpClient*, CONNID) override { return HR_OK; }
        EnHandleResult OnReceive(ITcpClient* pSender, CONNID, int iLength) override
        {
            // Pull mode keeps received bytes inside HP-Socket until upper layer fetches them.
            return m_owner.HandleClientReceive(pSender, iLength) ? HR_OK : HR_ERROR;
        }
        EnHandleResult OnSend(ITcpClient*, CONNID, const BYTE*, int) override { return HR_OK; }
        EnHandleResult OnClose(ITcpClient*, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
        {
            m_owner.HandleDisconnected(dwConnID, enOperation, iErrorCode);
            return HR_OK;
        }

    private:
        HPSocketTransport& m_owner;
    };

    HPSocketTransport::HPSocketTransport(Listener& listener)
        : m_listener(listener),
          m_serverListener(std::unique_ptr<ServerListenerImpl>(new ServerListenerImpl(*this))),
          m_clientListener(std::unique_ptr<ClientListenerImpl>(new ClientListenerImpl(*this))),
          m_server(m_serverListener.get()),
          m_client(m_clientListener.get()),
          m_started(false),
          m_connected(false),
          m_connId(0)
    {
    }

    HPSocketTransport::~HPSocketTransport()
    {
        Stop();
    }

    bool HPSocketTransport::Start(const SocketParameter& parameter)
    {
        Stop();
        m_parameter = parameter;
        return parameter.m_mode == EnumHSMSMode::EnumHSMSMode_Active ? StartActive(parameter) : StartPassive(parameter);
    }

    void HPSocketTransport::Stop()
    {
        m_connected = false;
        m_started = false;
        {
            std::lock_guard<std::mutex> lock(m_connMutex);
            m_connId = 0;
        }
        m_client->Stop();
        m_server->Stop();
    }

    bool HPSocketTransport::Send(const uint8_t* data, size_t length)
    {
        if (data == nullptr || length == 0 || !m_connected.load())
        {
            return false;
        }

        if (m_parameter.m_mode == EnumHSMSMode::EnumHSMSMode_Active)
        {
            std::lock_guard<std::mutex> lock(m_connMutex);
            return m_client->Send(data, static_cast<int>(length)) == TRUE;
        }

        std::lock_guard<std::mutex> lock(m_connMutex);
        return m_connId != 0 && m_server->Send(m_connId, data, static_cast<int>(length)) == TRUE;
    }

    bool HPSocketTransport::Disconnect()
    {
        if (m_parameter.m_mode == EnumHSMSMode::EnumHSMSMode_Active)
        {
            return m_client->Stop() == TRUE;
        }

        std::lock_guard<std::mutex> lock(m_connMutex);
        return m_connId != 0 && m_server->Disconnect(m_connId) == TRUE;
    }

    bool HPSocketTransport::Peek(uint8_t* data, size_t length)
    {
        if (data == nullptr || length == 0 || length > static_cast<size_t>(INT_MAX) || !m_connected.load())
        {
            return false;
        }

        if (m_parameter.m_mode == EnumHSMSMode::EnumHSMSMode_Active)
        {
            return m_client->Peek(data, static_cast<int>(length)) == FR_OK;
        }

        std::lock_guard<std::mutex> lock(m_connMutex);
        return m_connId != 0 && m_server->Peek(m_connId, data, static_cast<int>(length)) == FR_OK;
    }

    bool HPSocketTransport::Fetch(uint8_t* data, size_t length)
    {
        if (data == nullptr || length == 0 || length > static_cast<size_t>(INT_MAX) || !m_connected.load())
        {
            return false;
        }

        if (m_parameter.m_mode == EnumHSMSMode::EnumHSMSMode_Active)
        {
            return m_client->Fetch(data, static_cast<int>(length)) == FR_OK;
        }

        std::lock_guard<std::mutex> lock(m_connMutex);
        return m_connId != 0 && m_server->Fetch(m_connId, data, static_cast<int>(length)) == FR_OK;
    }

    bool HPSocketTransport::IsConnected() const
    {
        return m_connected.load();
    }

    bool HPSocketTransport::IsStarted() const
    {
        return m_started.load();
    }

    bool HPSocketTransport::StartActive(const SocketParameter& parameter)
    {
        if (!m_client->Start(parameter.m_ip, static_cast<USHORT>(parameter.m_port), TRUE))
        {
            ReportError("HP-Socket client start failed.");
            return false;
        }

        m_started = true;
        return true;
    }

    bool HPSocketTransport::StartPassive(const SocketParameter& parameter)
    {
        m_server->SetMaxConnectionCount(static_cast<DWORD>(parameter.m_maxConn));
        if (!m_server->Start(parameter.m_ip, static_cast<USHORT>(parameter.m_port)))
        {
            ReportError("HP-Socket server start failed.");
            return false;
        }

        m_started = true;
        return true;
    }

    void HPSocketTransport::HandleConnected(CONNID connId)
    {
        {
            std::lock_guard<std::mutex> lock(m_connMutex);
            m_connId = connId;
        }
        m_connected = true;
        m_listener.OnTransportConnected();
    }

    void HPSocketTransport::HandleDisconnected(CONNID connId, EnSocketOperation operation, int errorCode)
    {
        (void)connId;
        m_connected = false;
        if (m_parameter.m_mode == EnumHSMSMode::EnumHSMSMode_Active)
        {
            m_started = false;
        }
        {
            std::lock_guard<std::mutex> lock(m_connMutex);
            m_connId = 0;
        }

        if (errorCode != 0)
        {
            std::string message = "HP-Socket closed, operation=" + std::to_string(operation) +
                                  ", error=" + std::to_string(errorCode);
            ReportError(message.c_str());
        }

        m_listener.OnTransportDisconnected();
    }

    bool HPSocketTransport::HandleServerReceive(ITcpServer* sender, CONNID connId, int length)
    {
        auto* pullServer = ITcpPullServer::FromS(sender);
        if (pullServer == nullptr)
        {
            ReportError("HP-Socket server fetch interface unavailable.");
            return false;
        }

        m_listener.OnTransportReadable(static_cast<size_t>(length));
        return true;
    }

    bool HPSocketTransport::HandleClientReceive(ITcpClient* sender, int length)
    {
        auto* pullClient = ITcpPullClient::FromS(sender);
        if (pullClient == nullptr)
        {
            ReportError("HP-Socket client fetch interface unavailable.");
            return false;
        }


        m_listener.OnTransportReadable(static_cast<size_t>(length));
        return true;
    }

    void HPSocketTransport::ReportError(const char* message) const
    {
        m_listener.OnTransportError(message);
    }
}
