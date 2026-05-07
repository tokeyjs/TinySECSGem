#pragma once
#include "BaseItem.h"
#include<vector>
#include<string>

namespace TinySECSGem {


    /**
    * @brief SECS-II Boolean data item implementation
    *
    * Represents the Boolean data type (format code 'BOOLEAN') in the SECS/GEM protocol.
    * Stores boolean values using std::vector<bool>.
    * Used for device process states, flag bits, on/off status, or logic data.
    *
    * BooleanItem is a leaf SECS-II data node. Supports element append,
    * insert, delete, modification, serialization, and deserialization.
    * Can be nested inside ListItem or TopItem.
    */
    class TinySECSGEM_API BooleanItem : public BaseItem
    {
    private:
        std::vector<bool> m_data;   ///< Boolean array data

    public:

        /**
         * @brief Construct an empty Boolean item
         */
        BooleanItem();

        /**
         * @brief Construct a single-value Boolean item
         *
         * @param val Initial boolean value
         */
        BooleanItem(bool val);

        /**
         * @brief Copy constructor
         *
         * @param item Source BooleanItem
         */
        BooleanItem(const BooleanItem& item);

        /**
         * @brief Copy assignment
         *
         * @param item Source BooleanItem
         * @return Reference to this
         */
        BooleanItem& operator=(const BooleanItem& item);

        /**
         * @brief Assign a single boolean
         *
         * @param val New boolean value
         * @return Reference to this
         */
        BooleanItem& operator=(bool val);

        /**
         * @brief Destroy the Boolean item
         */
        ~BooleanItem();

        /**
         * @brief Clone this Boolean item
         *
         * @return Newly created BooleanItem (caller must free)
         */
        virtual BaseItem* clone();

        /**
         * @brief Access boolean at index
         *
         * @param index Element index
         * @return Boolean value at the position
         */
        const bool operator[](size_t index);

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
         * @return BooleanItem* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static BooleanItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print boolean values (debug)
         *
         * @param frontStr Indentation prefix
         * @return Formatted boolean string
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
         * @brief Append a boolean at the end
         *
         * @param val Boolean to append
         */
        void push_back(bool val);

        /**
         * @brief Remove the last boolean
         */
        void pop_back();

        /**
         * @brief Insert a boolean at the specified position
         *
         * @param pos Insertion index
         * @param val Boolean to insert
         */
        void insert(size_t pos, bool val);

        /**
         * @brief Erase the boolean at the specified position
         *
         * @param pos Index to erase
         */
        void erase(size_t pos);

        /**
        * @brief Get the boolean at index
        *
        * @param index Index (default 0, returns last if out of range)
        * @return Boolean value
        */
        bool get(size_t index = 0);

        /**
         * @brief Set the boolean at the specified index
         *
         * @param index Index (out-of-range appends at end)
         * @param val Value
         */
        void set(bool val, size_t index = 0);

        /**
         * @brief Set the boolean at the specified position
         *
         * @param index Element index
         * @param val New boolean value
         */
        void setAt(size_t index, bool val);
    };

}

