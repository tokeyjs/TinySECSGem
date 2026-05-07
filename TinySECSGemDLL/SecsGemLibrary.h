#pragma once

#include "SecsMessage.h"
#include "BaseItem.h"
#include "ListItem.h"
#include "TopItem.h"
#include <memory>

namespace TinySECSGem
{
    class HPSocketTransport;


    /**
    * @brief Event handler interface for SECS/GEM communication
    *
    * Implement this interface to receive callbacks for TCP connection,
    * HSMS SELECT state changes, received SECS messages, protocol errors,
    * and timeout events.
    */
    class TinySECSGEM_API ISecsGemHandler
    {
    public:
        virtual ~ISecsGemHandler() = default;

        /**
         * @brief Called when TCP connection is established
         */
        virtual void OnConnected() {}

        /**
        * @brief Called when HSMS SELECT state is achieved
        */
        virtual void OnSelected() {}

        /**
        * @brief Called when connection is disconnected
        */
        virtual void OnDisconnected() {}

        /**
        * @brief Called when a control message is received
        *
        * @param header SECS message header
        * @note The library automatically handles standard HSMS control messages internally.
        *       This callback is for informational purposes; no application handling required.
        */
        virtual void OnControlMessage(const BlockHeader& header) {}

        /**
        * @brief Called when a Primary data message is received
        *
        * @param message SECS message
        */
        virtual void OnPrimaryMessage(const SECSMessage& message) {}


        /**
        * @brief Called when a Secondary data message is received
        *
        * @param message SECS message
        */
        virtual void OnSecondaryMessage(const SECSMessage& message) {}

        /**
        * @brief Called when a protocol error occurs
        *
        * @param info Error description
        */
        virtual void OnProtocolError(const char* info) {}

        /**
        * @brief Called when a timer expires (T3/T5/T6/T7/T8, etc.)
        *
        * @param info Timer name
        * @param id Transaction ID associated with the timeout
        */
        virtual void OnTimeout(const char* info, uint32_t id) {}
    };


    /**
    * @brief SECS message validation interface
    *
    * Implement this interface to validate incoming SECS data messages.
    * When validation fails, the library automatically generates and sends
    * the appropriate Stream 9 error reply (S9F3, S9F5, S9F7).
    *
    * For example, if the application layer does not support S31 messages,
    * return false from CheckStream() for S31 messages, and the library
    * will automatically send an S9F3 error reply.
    */
    class ISECSMsgValidator
    {
    public:
        ISECSMsgValidator() {}

        virtual ~ISECSMsgValidator() = default;

        /**
         * @brief Validate the Stream number of the received message
         *
         * @param message The received SECS message
         * @return true Stream is supported, message processing continues
         * @return false Stream is not supported, library automatically sends S9F3 reply
         *
         * @note Called for every incoming data message. Return false to reject.
         */
        virtual bool CheckStream(const SECSMessage& message) { return true; }

        /**
        * @brief Validate the Function number of the received message
        *
        * @param message The received SECS message
        * @return true Function is supported, message processing continues
        * @return false Function is not supported, library automatically sends S9F5 reply
        */
        virtual bool CheckFunction(const SECSMessage& message) { return true; }

        /**
        * @brief Validate the data structure of the received message
        *
        * @param message The received SECS message
        * @return true Data structure is valid
        * @return false Data structure is invalid, library automatically sends S9F7 reply
        *
        * @note For standard SxFy messages, this validates the expected data format.
        *       Non-conforming messages trigger an S9F7 error reply.
        */
        virtual bool CheckMessageData(const SECSMessage& message) { return true; }

    };

    /**
    * @brief SECS/GEM communication controller
    *
    * This is the main API class for the TinySECSGem library.
    * It internally manages the transport layer, HSMS state machine,
    * timer management, and message send/receive flow.
    *
    * Users interact with it to configure communication parameters,
    * start/stop the connection, exchange SECS-II data messages,
    * and handle protocol events.
    */
    class TinySECSGEM_API SecsGem
    {
    public:
        SecsGem();
        ~SecsGem();

        /**
        * @brief Get the version string of TinySECSGemDLL
        *
        * @return Version string (e.g., "1.0.0")
        */
        static const char* GetVersion();

        /**
        * @brief Set socket communication parameters
        *
        * @param parameter Communication parameters including IP, port,
        *                  device ID, timeout values, and mode settings
        */
        void SetParameter(const SocketParameter& parameter);

        /**
        * @brief Get current socket communication parameters
        *
        * @return Currently configured communication parameters
        */
        SocketParameter GetParameter() const;

        /**
        * @brief Set the SECS/GEM event handler
        *
        * @param handler Pointer to the user-implemented event handler
        *
        * @note The handler object must outlive the SecsGem instance.
        *       Ensure its validity throughout the lifetime of this object.
        */
        void SetHandler(ISecsGemHandler* handler);

        /**
        * @brief Set the SECS message validator
        *
        * @param validator Pointer to the custom message validator
        *
        * @note Once set, the library takes ownership of the validator.
        *       Pass nullptr to restore the default pass-all validator.
        */
        void SetSECSMsgValidator(ISECSMsgValidator* validator);

        /**
        * @brief Start the SECS/GEM communication service
        *
        * Initializes the transport layer and starts the HSMS state machine
        * according to the configured parameters.
        *
        * @return true if started successfully
        * @return false if start failed
        */
        bool Start();

        /**
        * @brief Stop the SECS/GEM communication service
        *
        * Stops the transport layer and releases internal state resources.
        */
        void Stop();

        /**
        * @brief Send a raw SECS message
        *
        * @param message The SECS message to send
        * @return true if sent successfully
        * @return false if send failed
        */
        bool SendSECSMessage(const SECSMessage& message);

        /**
        * @brief Send a SECS-II data message
        *
        * @param stream Message stream number
        * @param function Message function number
        * @param waitReply Whether to wait for a reply (set W-bit)
        * @param data Message data item tree (optional, default = empty)
        * @return true if sent successfully
        * @return false if send failed
        */
        bool SendDataMessage(uint8_t stream, uint8_t function, bool waitReply, std::shared_ptr<BaseItem> data = nullptr);

        /**
        * @brief Send a Select.req control message
        *
        * Initiates the HSMS SELECT state transition.
        *
        * @return true if sent successfully
        * @return false if send failed
        */
        bool SendSelectRequest();

        /**
        * @brief Send a Deselect.req control message
        *
        * Initiates exit from the HSMS SELECTED state.
        *
        * @return true if sent successfully
        * @return false if send failed
        */
        bool SendDeselectRequest();

        /**
        * @brief Send a Linktest.req control message
        *
        * Checks whether the HSMS communication path is still alive.
        *
        * @return true if sent successfully
        * @return false if send failed
        */
        bool SendLinktestRequest();

        /**
        * @brief Send a Separate.req control message
        *
        * Notifies the peer to disconnect the current HSMS connection.
        *
        * @return true if sent successfully
        * @return false if send failed
        */
        bool SendSeparateRequest();

        /**
        * @brief Get the current HSMS connection state
        *
        * @return Current connection state enum
        */
        HsmsConnectionState GetConnectionState() const;

        /**
        * @brief Check whether the service is currently running
        *
        * @return true if running
        * @return false if not running
        */
        bool IsRunning() const;

        /**
        * @brief Check whether the session is in SELECTED state
        *
        * @return true if already SELECTED
        * @return false if not yet SELECTED
        */
        bool IsSelected() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
