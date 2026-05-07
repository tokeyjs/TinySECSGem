#pragma once
#include "BaseItem.h"
#include<vector>
#include<string>
#include<cstdint>

namespace TinySECSGem {

    /**
    * @brief SECS-II 1-byte unsigned integer data item implementation
    *
    * Represents the U1 (UInt1Byte) format in the SECS/GEM protocol.
    * Stores 8-bit unsigned integers using std::vector<uint8_t>.
    * Used for flag bits, small-range counts, status codes, or
    * binary parameters.
    *
    * UInt1ByteItem is a leaf SECS-II numeric node. Supports element append,
    * insert, delete, modification, serialization, and deserialization.
    * Can be nested inside ListItem or TopItem.
    */
    class TinySECSGEM_API UInt1ByteItem : public BaseItem
    {
    private:
        std::vector<uint8_t> m_data;   ///< 1-byte unsigned integer array

    public:

        /**
         * @brief Construct an empty UInt1Byte item
         */
        UInt1ByteItem();

        /**
         * @brief Construct a single-value UInt1Byte item
         *
         * @param val Initial 8-bit unsigned integer
         */
        UInt1ByteItem(uint8_t val);

        /**
         * @brief Copy constructor
         *
         * @param item Source UInt1ByteItem
         */
        UInt1ByteItem(const UInt1ByteItem& item);

        /**
         * @brief Copy assignment
         *
         * @param item Source UInt1ByteItem
         * @return Reference to this
         */
        UInt1ByteItem& operator=(const UInt1ByteItem& item);

        /**
         * @brief Assign a single value
         *
         * @param val New 8-bit unsigned integer
         * @return Reference to this
         */
        UInt1ByteItem& operator=(uint8_t val);

        /**
         * @brief Destroy the UInt1Byte item
         */
        ~UInt1ByteItem();

        /**
         * @brief Clone this UInt1Byte item
         *
         * @return Newly created UInt1ByteItem (caller must free)
         */
        virtual BaseItem* clone();

        /**
         * @brief Access element at index (unchecked)
         *
         * @param index Element index
         * @return Reference to the element
         *
         * @warning Out-of-bounds access has undefined behavior
         */
        uint8_t& operator[](size_t index);

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
         * @return UInt1ByteItem* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static UInt1ByteItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print unsigned integer values (debug)
         *
         * @param frontStr Indentation prefix
         * @return Formatted unsigned integer string
         */
        virtual std::string print(std::string frontStr = "") const;

        /**
         * @brief Get the number of elements
         *
         * @return Element count
         */
        virtual size_t GetArraySize() const;

        /**
         * @brief Clear all data
         */
        virtual void clear();

        /**
         * @brief Append an unsigned integer at the end
         *
         * @param val 8-bit unsigned integer to append
         */
        void push_back(uint8_t val);

        /**
         * @brief Remove the last unsigned integer
         */
        void pop_back();

        /**
         * @brief Set the value at the specified index
         *
         * @param index Index (out-of-range appends at end)
         * @param val Value
         */
        void set(uint8_t val, size_t index = 0);

        /**
         * @brief Get the value at index
         *
         * @param index Index (default 0, returns last if out of range)
         * @return Unsigned integer value
         */
        uint8_t get(size_t index = 0);

        /**
         * @brief Insert an unsigned integer at the specified position
         *
         * @param pos Insertion index
         * @param val 8-bit unsigned integer to insert
         */
        void insert(size_t pos, uint8_t val);

        /**
         * @brief Erase the element at the specified position
         *
         * @param pos Index to erase
         */
        void erase(size_t pos);
    };

}
