#pragma once

#include "TinySECSGem.h"
#include <HPSocket.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <vector>

namespace TinySECSGem
{
    // Transport adapter that hides HP-Socket details from the HSMS session layer.
    class HPSocketTransport
    {
    public:
        class Listener
        {
        public:
            virtual ~Listener() = default;
            // TCP is connected, but HSMS may still be in NOT SELECTED state.
            virtual void OnTransportConnected() = 0;
            virtual void OnTransportDisconnected() = 0;
            // Pull mode stores bytes inside HP-Socket until upper layer peeks/fetches them.
            virtual void OnTransportReadable(size_t length) = 0;
            virtual void OnTransportError(const char* message) = 0;
        };

        explicit HPSocketTransport(Listener& listener);
        ~HPSocketTransport();

        // Start active(client) or passive(server) mode according to SocketParameter.
        bool Start(const SocketParameter& parameter);
        void Stop();
        bool Send(const uint8_t* data, size_t length);
        bool Disconnect();
        bool Peek(uint8_t* data, size_t length);
        bool Fetch(uint8_t* data, size_t length);

        bool IsConnected() const;
        bool IsStarted() const;

    private:
        class ServerListenerImpl;
        class ClientListenerImpl;

        bool StartActive(const SocketParameter& parameter);
        bool StartPassive(const SocketParameter& parameter);
        void HandleConnected(CONNID connId);
        void HandleDisconnected(CONNID connId, EnSocketOperation operation, int errorCode);
        bool HandleServerReceive(ITcpServer* sender, CONNID connId, int length);
        bool HandleClientReceive(ITcpClient* sender, int length);
        void ReportError(const char* message) const;

        Listener& m_listener;
        std::unique_ptr<ServerListenerImpl> m_serverListener;
        std::unique_ptr<ClientListenerImpl> m_clientListener;
        CTcpPullServerPtr m_server;
        CTcpPullClientPtr m_client;
        SocketParameter m_parameter;
        std::atomic<bool> m_started;
        std::atomic<bool> m_connected;
        mutable std::mutex m_connMutex;
        CONNID m_connId;
    };
}
