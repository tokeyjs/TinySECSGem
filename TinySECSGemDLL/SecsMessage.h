#pragma once

#include "BaseItem.h"

#include <memory>
#include <vector>

namespace TinySECSGem
{
#pragma pack(push, 1)
    /**
    * @brief HSMS 10-byte fixed message header structure
    *
    * Maps directly to the SEMI E37 HSMS Block Header wire format.
    * Provides field accessors, primary/secondary message detection,
    * session control classification, and factory methods for standard
    * HSMS control message headers.
    */
    struct TinySECSGEM_API BlockHeader
    {
#if SECS_ENDIAN == SECS_LITTLE_ENDIAN
        BYTE m_hiDevId;          ///< Session ID high byte
        BYTE m_loDevId;          ///< Session ID low byte
        BYTE m_hiMsgId : 7;      ///< Stream number (upper 7 bits)
        BYTE m_w : 1;            ///< W-Bit, reply wait flag
        BYTE m_loMsgId;          ///< Function number or control status code
        BYTE m_pType;            ///< Protocol type (0 for SECS-II)
        BYTE m_sType;            ///< Session control message type or data message indicator
        BYTE m_system[4];        ///< System Bytes transaction identifier
#else
        BYTE m_hiDevId;          ///< Session ID high byte
        BYTE m_loDevId;          ///< Session ID low byte
        BYTE m_w : 1;            ///< W-Bit, reply wait flag
        BYTE m_hiMsgId : 7;      ///< Stream number (upper 7 bits)
        BYTE m_loMsgId;          ///< Function number or control status code
        BYTE m_pType;            ///< Protocol type (0 for SECS-II)
        BYTE m_sType;            ///< Session control message type or data message indicator
        BYTE m_system[4];        ///< System Bytes transaction identifier
#endif

    /**
     * @brief Construct a default message header (zero-filled)
     */
        BlockHeader();

        /**
         * @brief Set the Session ID
         * @param sessionId Session ID value
         */
        void setSessionId(int sessionId);

        /**
         * @brief Get the Session ID
         * @return int Current Session ID
         */
        int getSessionId() const;

        /**
         * @brief Set the Stream number
         * @param stream Stream value
         */
        void setStreamNum(int stream);

        /**
         * @brief Get the Stream number
         * @return unsigned int Current Stream value
         */
        unsigned int getStreamNum() const;

        /**
         * @brief Set the Function number
         * @param function Function value
         */
        void setFunctionNum(int function);

        /**
         * @brief Get the Function number
         * @return unsigned int Current Function value
         */
        unsigned int getFunctionNum() const;

        /**
         * @brief Get the combined Stream/Function number
         * @return unsigned int Combined message number
         */
        unsigned int getStreamFunctionNum() const;

        /**
         * @brief Set the protocol type (PType)
         * @param pType Protocol type value
         */
        void setPType(int pType);

        /**
         * @brief Get the protocol type (PType)
         * @return int Current PType value
         */
        int getPType() const;

        /**
         * @brief Set the session type (SType)
         * @param sType Session control type value
         */
        void setSType(int sType);

        /**
         * @brief Get the session type (SType)
         * @return int Current SType value
         */
        int getSType() const;

        /**
         * @brief Set the W-Bit reply wait flag
         * @param waitBit true if a reply is expected, false otherwise
         */
        void setNeedReply(bool waitBit);

        /**
         * @brief Check whether the message expects a reply (W-Bit set)
         * @return true if reply is expected
         * @return false if no reply expected
         */
        bool isReply() const;

        /**
         * @brief Set the transaction ID (System Bytes)
         * @param transactionId Transaction number
         */
        void setTransactionNum(uint32_t transactionId);

        /**
         * @brief Get the transaction ID
         * @return uint32_t Current transaction number
         */
        uint32_t getTransactionNum() const;

        /**
         * @brief Check whether this is a Primary message
         * @return true for Primary (odd function number)
         * @return false for Secondary (even function number)
         */
        bool isPrimary() const;

        /**
         * @brief Check whether this is a Secondary message
         * @return true for Secondary (even function number)
         * @return false for Primary (odd function number)
         */
        bool isSecondary() const;

        /**
         * @brief Check whether this is an HSMS session control message
         * @return true for control messages
         * @return false for data messages
         */
        bool isSession() const;

        /**
         * @brief Check whether this control message expects a session-level response
         * @return true if response expected (Select/Deselect/Linktest)
         * @return false otherwise
         */
        bool wantSessionRsp() const;

        /**
         * @brief Check whether this header matches the expected reply to another header
         *
         * Used for transaction matching between primary and secondary messages.
         *
         * @param other The target message header
         * @return true if this is the expected reply to 'other'
         * @return false if not a match
         */
        bool isExpect(const BlockHeader& other) const;

        /**
         * @brief Build a standard SECS-II data message header
         */
        void Set_DataMessage(int sessionId, bool waitBit, int stream, int function, uint32_t transactionId);

        /**
         * @brief Build a Secondary message header from the corresponding Primary header
         */
        void Set_DataMessage_GenSecondaryMsgHeader(const BlockHeader& primaryMsgHeader);

        /**
         * @brief Build a Select.req session establishment request header
         */
        void Set_SelectReq(int sessionId, uint32_t transactionId);

        /**
         * @brief Build a Select.rsp session establishment response header
         */
        void Set_SelectRsp(int sessionId, int selectStatus, uint32_t transactionId);

        /**
         * @brief Build a Deselect.req session exit request header
         */
        void Set_DeselectReq(int sessionId, uint32_t transactionId);

        /**
         * @brief Build a Deselect.rsp session exit response header
         */
        void Set_DeselectRsp(int sessionId, int deselectStatus, uint32_t transactionId);

        /**
         * @brief Build a Linktest.req link test request header
         */
        void Set_LinktestReq(uint32_t transactionId);

        /**
         * @brief Build a Linktest.rsp link test response header
         */
        void Set_LinktestRsp(uint32_t transactionId);

        /**
         * @brief Build a Reject.req rejection request header
         */
        void Set_RejectReq(int sessionId, int byte2, int reasonCode, uint32_t transactionId);

        /**
         * @brief Build a Separate.req disconnect request header
         */
        void Set_SeparateReq(int sessionId, uint32_t transactionId);
};
#pragma pack(pop)

    /**
    * @brief SECS/GEM protocol message wrapper class
    *
    * Represents a complete SECS/GEM message containing an HSMS protocol
    * header (BlockHeader) and optional SECS-II data body (BaseItem).
    *
    * Provides header management, data body handling, protocol attribute
    * queries, and serialization — it is the fundamental message carrier
    * for all send, receive, parse, and build operations in the library.
    */
    class TinySECSGEM_API SECSMessage
    {
    public:

        /**
         * @brief Construct an empty SECS message
         */
        SECSMessage();

        /**
         * @brief Set the message header (by reference)
         *
         * @param head BlockHeader object reference
         */
        void SetHead(const BlockHeader& head);

        /**
         * @brief Set the message header (by pointer)
         *
         * @param head BlockHeader object pointer
         */
        void SetHead(const BlockHeader* head);

        /**
         * @brief Get a mutable reference to the message header
         *
         * @return BlockHeader& Reference to the current header
         */
        BlockHeader& GetHead();

        /**
         * @brief Get a const reference to the message header
         *
         * @return const BlockHeader& Const reference to the current header
         */
        const BlockHeader& GetHead() const;

        /**
         * @brief Get a pointer to the message header
         *
         * @return BlockHeader* Pointer to the current header
         */
        BlockHeader* GetHeadPtr();

        /**
         * @brief Get the message data body
         *
         * @return std::shared_ptr<BaseItem> The SECS-II data item root node
         */
        std::shared_ptr<BaseItem> GetData() const;

        /**
         * @brief Set the message data body (shared_ptr)
         *
         * @param data SECS-II data item shared pointer
         */
        void SetData(std::shared_ptr<BaseItem> data);

        /**
         * @brief Set the message data body (raw pointer)
         *
         * @param data SECS-II data item raw pointer
         *
         * @note The function takes ownership of the object and wraps it in a shared_ptr.
         */
        void SetData(BaseItem* data);

        /**
         * @brief Check whether the message has a data body
         *
         * @return true if data item is null (empty)
         * @return false if data item is non-null
         */
        bool isItemEmpty() const;

        /**
         * @brief Check whether this is a data message (not a control message)
         *
         * Data messages are SECS-II business messages in the form SxFy.
         *
         * @return true if data message
         * @return false if control message
         */
        bool isDataMessage() const;

        /**
         * @brief Check whether the message expects a reply (W-Bit)
         *
         * Based on the W-Bit flag in the message header.
         *
         * @return true if reply expected
         * @return false if no reply expected
         */
        bool isNeedReply() const;

        /**
         * @brief Get the message Stream number
         *
         * @return int Current Stream value
         */
        int GetStream() const;

        /**
         * @brief Get the message Function number
         *
         * @return int Current Function value
         */
        int GetFunction() const;

        /**
         * @brief Get the Session ID
         *
         * @return int Current Session ID
         */
        int GetSessionID() const;

        /**
         * @brief Get the Transaction ID
         *
         * @return uint32_t Current Transaction ID
         */
        uint32_t GetTransactionID() const;

        /**
         * @brief Serialize the HSMS message payload
         *
         * Produces: 10-byte HSMS header + optional SECS-II data body.
         *
         * @param buffer Output buffer
         * @param bufferSize Output buffer size
         * @return size_t Number of bytes written
         */
        size_t SerializationMsg(BYTE* buffer, size_t bufferSize) const;

        /**
         * @brief Serialize the complete network transmission frame
         *
         * Produces: 4-byte big-endian length prefix + HSMS message payload.
         *
         * @return std::vector<uint8_t> Complete wire-format byte stream
         */
        std::vector<uint8_t> SerializeForWire() const;

    private:
        BlockHeader m_head;                       ///< HSMS message header
        std::shared_ptr<BaseItem> m_data;        ///< SECS-II data item
    };

    /**
    * @brief SECS/GEM message byte stream parser
    *
    * Parses received HSMS network byte streams into SECS/GEM protocol
    * message objects. Supports full wire frame parsing, HSMS payload
    * parsing, and protocol header parsing.
    *
    * All interfaces are static — no instantiation required.
    */
    class TinySECSGEM_API SecsMessageParser
    {
    public:

        /**
         * @brief Parse HSMS message payload (without 4-byte length prefix)
         *
         * Input data format: 10-byte HSMS header + optional SECS-II data body.
         *
         * @param data Input byte stream buffer
         * @param length Input byte stream length
         * @param ok Output flag: true on success, false on failure
         * @return std::shared_ptr<SECSMessage> Parsed SECS message (nullptr on failure)
         */
        static std::shared_ptr<SECSMessage> ParsePayload(const BYTE* data, size_t length, bool& ok);

        /**
         * @brief Parse the HSMS protocol header (BlockHeader)
         *
         * Extracts the 10-byte BlockHeader from the input byte stream.
         *
         * @param data Input byte stream buffer
         * @param length Input byte stream length
         * @param ok Output flag: true on success, false on failure
         * @return BlockHeader Parsed header object
         */
        static BlockHeader ParseHeader(const BYTE* data, size_t length, bool& ok);

        /**
         * @brief Parse a complete network transmission frame
         *
         * Parses a full HSMS network frame with the format:
         * 4-byte big-endian length prefix + HSMS message payload.
         *
         * @param data Input byte stream buffer
         * @param length Input byte stream length
         * @param ok Output flag: true on success, false on failure
         * @return std::shared_ptr<SECSMessage> Parsed SECS message (nullptr on failure)
         */
        static std::shared_ptr<SECSMessage> ParseWireMessage(const BYTE* data, size_t length, bool& ok);
    };
}
