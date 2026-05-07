#pragma once
#include "BaseItem.h"

#include<vector>
#include<string>
#include<cstdint>

namespace TinySECSGem {


    /**
    * @brief SECS-II 8-byte floating-point data item implementation
    *
    * Represents the F8 (Float8Byte) format in the SECS/GEM protocol.
    * Stores double-precision floats using std::vector<double>.
    *
    * Float8ByteItem is a leaf SECS-II numeric node. Supports element append,
    * insert, delete, modification, serialization, and deserialization.
    * Can be nested inside ListItem or TopItem.
    */
    class TinySECSGEM_API Float8ByteItem : public BaseItem
    {
    private:
        std::vector<double> m_data;   ///< 8-byte floating-point array

    public:

        /**
         * @brief Construct an empty Float8Byte item
         */
        Float8ByteItem();

        /**
         * @brief Construct a single-value Float8Byte item
         *
         * @param val Initial double value
         */
        Float8ByteItem(double val);

        /**
         * @brief Copy constructor
         *
         * @param item Source Float8ByteItem
         */
        Float8ByteItem(const Float8ByteItem& item);

        /**
         * @brief Copy assignment
         *
         * @param item Source Float8ByteItem
         * @return Reference to this
         */
        Float8ByteItem& operator=(const Float8ByteItem& item);

        /**
         * @brief Assign a single value
         *
         * @param val New double value
         * @return Reference to this
         */
        Float8ByteItem& operator=(double val);

        /**
         * @brief Destroy the Float8Byte item
         */
        ~Float8ByteItem();

        /**
         * @brief Clone this Float8Byte item
         *
         * @return Newly created Float8ByteItem (caller must free)
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
        double& operator[](size_t index);

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
         * @return Float8ByteItem* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static Float8ByteItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print double values (debug)
         *
         * @param frontStr Indentation prefix
         * @return Formatted double string
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
         * @brief Append a double at the end
         *
         * @param val Double to append
         */
        void push_back(double val);

        /**
         * @brief Remove the last double
         */
        void pop_back();

        /**
         * @brief Set the value at the specified index
         *
         * @param index Index (out-of-range appends at end)
         * @param val Value
         */
        void set(double val, size_t index = 0);

        /**
        * @brief Get the value at index
        *
        * @param index Index (default 0, returns last if out of range)
        * @return Double value
        */
        double get(size_t index = 0);

        /**
         * @brief Insert a double at the specified position
         *
         * @param pos Insertion index
         * @param val Double to insert
         */
        void insert(size_t pos, double val);

        /**
         * @brief Erase the element at the specified position
         *
         * @param pos Index to erase
         */
        void erase(size_t pos);
    };

}

