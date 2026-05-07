#pragma once
#include "BaseItem.h"

#include<vector>
#include<string>
#include<cstdint>

namespace TinySECSGem {

    /**
    * @brief SECS-II 4-byte signed integer data item implementation
    *
    * Represents the I4 (Int4Byte) format in the SECS/GEM protocol.
    * Stores 32-bit signed integers using std::vector<int32_t>.
    *
    * Int4ByteItem is a leaf SECS-II numeric node. Supports element append,
    * insert, delete, modification, serialization, and deserialization.
    * Can be nested inside ListItem or TopItem.
    */
    class TinySECSGEM_API Int4ByteItem : public BaseItem
    {
    private:
        std::vector<int32_t> m_data;   ///< 4-byte signed integer array

    public:

        /**
         * @brief Construct an empty Int4Byte item
         */
        Int4ByteItem();

        /**
         * @brief Construct a single-value Int4Byte item
         *
         * @param val Initial 32-bit signed integer
         */
        Int4ByteItem(int32_t val);

        /**
         * @brief Copy constructor
         *
         * @param item Source Int4ByteItem
         */
        Int4ByteItem(const Int4ByteItem& item);

        /**
         * @brief Copy assignment
         *
         * @param item Source Int4ByteItem
         * @return Reference to this
         */
        Int4ByteItem& operator=(const Int4ByteItem& item);

        /**
         * @brief Assign a single value
         *
         * @param val New 32-bit signed integer
         * @return Reference to this
         */
        Int4ByteItem& operator=(int32_t val);

        /**
         * @brief Destroy the Int4Byte item
         */
        ~Int4ByteItem();

        /**
         * @brief Clone this Int4Byte item
         *
         * @return Newly created Int4ByteItem (caller must free)
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
        int32_t& operator[](size_t index);

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
         * @return Int4ByteItem* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static Int4ByteItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

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
         * @param val 32-bit signed integer to append
         */
        void push_back(int32_t val);

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
        void set(int32_t val, size_t index = 0);

        /**
         * @brief Get the value at index
         *
         * @param index Index (default 0, returns last if out of range)
         * @return Integer value
         */
        int32_t get(size_t index = 0);

        /**
         * @brief Insert an integer at the specified position
         *
         * @param pos Insertion index
         * @param val 32-bit signed integer to insert
         */
        void insert(size_t pos, int32_t val);

        /**
         * @brief Erase the element at the specified position
         *
         * @param pos Index to erase
         */
        void erase(size_t pos);
    };

}

