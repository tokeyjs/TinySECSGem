#pragma once
#include "BaseItem.h"

#include<vector>
#include<string>
#include<cstdint>

namespace TinySECSGem {

    /**
    * @brief SECS-II 4-byte floating-point data item implementation
    *
    * Represents the F4 (Float4Byte) format in the SECS/GEM protocol.
    * Stores single-precision floats using std::vector<float>.
    *
    * Float4ByteItem is a leaf SECS-II numeric node. Supports element append,
    * insert, delete, modification, serialization, and deserialization.
    * Can be nested inside ListItem or TopItem.
    */
    class TinySECSGEM_API Float4ByteItem : public BaseItem
    {
    private:
        std::vector<float> m_data;   ///< 4-byte floating-point array

    public:

        /**
         * @brief Construct an empty Float4Byte item
         */
        Float4ByteItem();

        /**
         * @brief Construct a single-value Float4Byte item
         *
         * @param val Initial float value
         */
        Float4ByteItem(float val);

        /**
         * @brief Copy constructor
         *
         * @param item Source Float4ByteItem
         */
        Float4ByteItem(const Float4ByteItem& item);

        /**
         * @brief Copy assignment
         *
         * @param item Source Float4ByteItem
         * @return Reference to this
         */
        Float4ByteItem& operator=(const Float4ByteItem& item);

        /**
         * @brief Assign a single value
         *
         * @param val New float value
         * @return Reference to this
         */
        Float4ByteItem& operator=(float val);

        /**
         * @brief Destroy the Float4Byte item
         */
        ~Float4ByteItem();

        /**
         * @brief Clone this Float4Byte item
         *
         * @return Newly created Float4ByteItem (caller must free)
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
        float& operator[](size_t index);

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
         * @return Float4ByteItem* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static Float4ByteItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print float values (debug)
         *
         * @param frontStr Indentation prefix
         * @return Formatted float string
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
         * @brief Append a float at the end
         *
         * @param val Float to append
         */
        void push_back(float val);

        /**
         * @brief Remove the last float
         */
        void pop_back();

        /**
         * @brief Set the value at the specified index
         *
         * @param index Index (out-of-range appends at end)
         * @param val Value
         */
        void set(float val, size_t index = 0);

        /**
        * @brief Get the value at index
        *
        * @param index Index (default 0, returns last if out of range)
        * @return Float value
        */
        float get(size_t index = 0);

        /**
         * @brief Insert a float at the specified position
         *
         * @param pos Insertion index
         * @param val Float to insert
         */
        void insert(size_t pos, float val);

        /**
         * @brief Erase the element at the specified position
         *
         * @param pos Index to erase
         */
        void erase(size_t pos);
    };


}

