#pragma once
#include "BaseItem.h"


namespace TinySECSGem {


    /**
    * @brief SECS-II Binary data item implementation
    *
    * Represents the Binary data type (format code 'B') in the SECS/GEM protocol.
    * Stores raw byte arrays using std::vector<BYTE>.
    * Used for device status bytes, control identifiers, or any raw binary data.
    *
    * BinaryItem is a leaf SECS-II data node. Supports byte append,
    * insert, delete, replace, serialization, and deserialization.
    * Can be nested inside ListItem or TopItem.
    */
    class TinySECSGEM_API BinaryItem : public BaseItem
    {
    private:
        std::vector<BYTE> m_data;   ///< Binary byte array data

    public:

        /**
         * @brief Construct an empty Binary item
         */
        BinaryItem();

        /**
         * @brief Construct a single-byte Binary item
         *
         * @param val Initial byte value
         */
        BinaryItem(BYTE val);

        /**
         * @brief Construct a Binary item from a byte range (end exclusive)
         *
         * @param start Start byte pointer
         * @param end End byte pointer (exclusive)
         */
        BinaryItem(BYTE* start, BYTE* end);

        /**
         * @brief Copy constructor
         *
         * @param item Source BinaryItem
         */
        BinaryItem(const BinaryItem& item);

        /**
         * @brief Copy assignment
         *
         * @param item Source BinaryItem
         * @return Reference to this
         */
        BinaryItem& operator=(const BinaryItem& item);

        /**
         * @brief Assign a single byte
         *
         * @param val New byte value
         * @return Reference to this
         */
        BinaryItem& operator=(BYTE val);

        /**
         * @brief Destroy the Binary item
         */
        ~BinaryItem();

        /**
         * @brief Clone this Binary item
         *
         * @return Newly created BinaryItem (caller must free)
         */
        virtual BaseItem* clone();

        /**
         * @brief Get the serialized byte size
         *
         * @return Total serialized bytes
         */
        virtual size_t totalByteSize() const;

        /**
         * @brief Serialize to SECS-II byte stream
         *
         * @param buff Output buffer
         * @param buffSize Buffer size
         * @return Actual bytes written, or negative on failure
         */
        virtual int Serialization(BYTE* buff, size_t buffSize);

        /**
         * @brief Deserialize from byte stream
         *
         * @param bufStart Start of buffer
         * @param bufEnd End of buffer (exclusive)
         * @return BinaryItem* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static BinaryItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print content in hex format (debug)
         *
         * @param frontStr Indentation prefix
         * @return Formatted hex string
         */
        virtual std::string print(std::string frontStr = "") const;

        /**
         * @brief Get the number of bytes
         *
         * @return Byte count
         */
        virtual size_t GetArraySize() const;

        /**
         * @brief Clear all data
         */
        virtual void clear();

        /**
         * @brief Append a byte at the end
         *
         * @param val Byte to append
         */
        void push_back(BYTE val);

        /**
         * @brief Remove the last byte
         */
        void pop_back();

        /**
         * @brief Insert a byte at the specified position
         *
         * @param pos Insertion index
         * @param val Byte to insert
         */
        void insert(size_t pos, BYTE val);

        /**
         * @brief Erase the byte at the specified position
         *
         * @param pos Index to erase
         */
        void erase(size_t pos);

        /**
         * @brief Replace content with a byte range (end exclusive)
         *
         * @param start Start byte pointer
         * @param end End byte pointer (exclusive)
         */
        void set(BYTE* start, BYTE* end);

        /**
         * @brief Set the byte at the specified index
         *
         * @param index Index (out-of-range appends at end)
         * @param val Value
         */
        void set(BYTE val, size_t index = 0);

        /**
         * @brief Get the byte at index
         *
         * @param index Index (default 0, returns last if out of range)
         * @return Byte value
         */
        BYTE get(size_t index = 0);

        /**
         * @brief Access byte at index (unchecked)
         *
         * @param index Byte index
         * @return Reference to the byte
         *
         * @warning Out-of-bounds access has undefined behavior
         */
        BYTE& operator[](size_t index);
    };

}

