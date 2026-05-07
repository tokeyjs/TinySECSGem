#pragma once
#include "BaseItem.h"

#include<vector>
#include<string>
#include<cstdint>

namespace TinySECSGem {

    /**
    * @brief SECS-II 8-byte signed integer data item implementation
    *
    * Represents the I8 (Int8Byte) format in the SECS/GEM protocol.
    * Stores 64-bit signed integers using std::vector<int64_t>.
    * Used for large-range values, high-precision timestamps, or
    * unique identifiers.
    *
    * Int8ByteItem is a leaf SECS-II numeric node. Supports element append,
    * insert, delete, modification, serialization, and deserialization.
    * Can be nested inside ListItem or TopItem.
    */
    class TinySECSGEM_API Int8ByteItem : public BaseItem
    {
    private:
        std::vector<int64_t> m_data;   ///< 8-byte signed integer array

    public:

        /**
         * @brief Construct an empty Int8Byte item
         */
        Int8ByteItem();

        /**
         * @brief Construct a single-value Int8Byte item
         *
         * @param val Initial 64-bit signed integer
         */
        Int8ByteItem(int64_t val);

        /**
         * @brief Copy constructor
         *
         * @param item Source Int8ByteItem
         */
        Int8ByteItem(const Int8ByteItem& item);

        /**
         * @brief Copy assignment
         *
         * @param item Source Int8ByteItem
         * @return Reference to this
         */
        Int8ByteItem& operator=(const Int8ByteItem& item);

        /**
         * @brief Assign a single value
         *
         * @param val New 64-bit signed integer
         * @return Reference to this
         */
        Int8ByteItem& operator=(int64_t val);

        /**
         * @brief Destroy the Int8Byte item
         */
        ~Int8ByteItem();

        /**
         * @brief Clone this Int8Byte item
         *
         * @return Newly created Int8ByteItem (caller must free)
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
        int64_t& operator[](size_t index);

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
         * @return Int8ByteItem* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static Int8ByteItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print integer values (debug)
         *
         * @param frontStr Indentation prefix
         * @return Formatted integer string
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
         * @brief Append an integer at the end
         *
         * @param val 64-bit signed integer to append
         */
        void push_back(int64_t val);

        /**
         * @brief Remove the last integer
         */
        void pop_back();

        /**
         * @brief Set the value at the specified index
         *
         * @param index Index (out-of-range appends at end)
         * @param val Value
         */
        void set(int64_t val, size_t index = 0);

        /**
         * @brief Get the value at index
         *
         * @param index Index (default 0, returns last if out of range)
         * @return Integer value
         */
        int64_t get(size_t index = 0);

        /**
         * @brief Insert an integer at the specified position
         *
         * @param pos Insertion index
         * @param val 64-bit signed integer to insert
         */
        void insert(size_t pos, int64_t val);

        /**
         * @brief Erase the element at the specified position
         *
         * @param pos Index to erase
         */
        void erase(size_t pos);
    };

}


