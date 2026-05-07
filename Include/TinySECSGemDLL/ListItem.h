#pragma once
#include "BaseItem.h"

#include<vector>
#include<string>
#include<cstdint>

#include "BinaryItem.h"
#include "BooleanItem.h"
#include "ASCIIItem.h"
#include "UnicodeItem.h"
#include "JIS8Item.h"
#include "Int1ByteItem.h"
#include "Int2ByteItem.h"
#include "Int4ByteItem.h"
#include "Int8ByteItem.h"
#include "UInt1ByteItem.h"
#include "UInt2ByteItem.h"
#include "UInt4ByteItem.h"
#include "UInt8ByteItem.h"
#include "Float4ByteItem.h"
#include "Float8ByteItem.h"

namespace TinySECSGem {

    /**
    * @brief SECS-II LIST data item implementation
    *
    * Represents the LIST compound data structure in the SECS/GEM protocol.
    * LIST is a variable-length container type for organizing multiple
    * SECS-II data items. Supports nested structures (List of List),
    * making it one of the core node types in the SECS data tree.
    */
    class TinySECSGEM_API ListItem : public BaseItem
    {
    private:
        std::vector<BaseItem*> m_data;   ///< Child item container (owning raw pointers)

    public:

        /**
         * @brief Construct an empty LIST
         */
        ListItem();

        /**
         * @brief Construct a single-element LIST
         * @param val Initial child item (ListItem takes ownership)
         */
        ListItem(BaseItem* val);

        /**
         * @brief Copy constructor (deep copy)
         * @param item Source LIST
         */
        ListItem(const ListItem& item);

        /**
         * @brief Copy assignment (deep copy)
         * @param item Source LIST
         */
        ListItem& operator=(const ListItem& item);

        /**
         * @brief Assign a single child (clears current content, inserts val)
         * @param val Item pointer (ListItem takes ownership)
         */
        ListItem& operator=(BaseItem* val);

        /**
         * @brief Destructor, releases all child items
         */
        ~ListItem();

        /**
         * @brief Clone this LIST (deep copy)
         * @return New LIST pointer (caller owns the memory)
         */
        virtual BaseItem* clone();

        /**
         * @brief Compute total serialized byte size (including all children)
         */
        virtual size_t totalByteSize() const;

        /**
         * @brief Serialize LIST to SECS-II byte stream
         * @param buff Output buffer
         * @param buffSize Buffer size
         * @return Actual bytes written
         */
        virtual int Serialization(BYTE* buff, size_t buffSize);

        /**
         * @brief Deserialize a LIST from byte stream
         * @param bufStart Start buffer pointer
         * @param bufEnd End buffer pointer (exclusive)
         * @return Parsed ListItem pointer (nullptr on failure)
         */
        static ListItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
        * @brief Convert to human-readable string (debug)
        * @param frontStr Prefix for indentation
        * @return Formatted string
        */
        virtual std::string print(std::string frontStr = "") const;

        /**
         * @brief Get the number of child items
         * @return Child count
         */
        virtual size_t GetArraySize() const;

        /**
         * @brief Clear all child items and release memory
         */
        virtual void clear();

        /**
         * @brief Append a child at the end
         * @param val Item pointer (ListItem takes ownership)
         */
        void push_back(BaseItem* val);

        /**
         * @brief Remove the last child
         */
        void pop_back();

        /**
         * @brief Set the child at the specified index
         *
         * @param index Index (out-of-range inserts at end)
         * @param val Value (old value is released)
         */
        void set(BaseItem* val, size_t index = 0);

        /**
         * @brief Insert a child at the specified position
         * @param pos Insertion index
         * @param val Item pointer (ListItem takes ownership)
         */
        void insert(size_t pos, BaseItem* val);

        /**
         * @brief Erase the child at the specified position
         * @param pos Index to erase
         */
        void erase(size_t pos);

        /**
         * @brief Get the child at the specified index, cast to target type
         * @tparam T Target type
         * @param index Index
         * @return Cast pointer, or nullptr on failure
         */
        template<typename T>
        T* get(size_t index) const {
            if (index >= GetArraySize()) return nullptr;
            return m_data[index]->as<T>();
        }

        /**
         * @brief Check whether the child at index is of the specified type
         * @tparam T Target type
         * @param index Index
         * @return true if type matches
         * @return false if type mismatch or out of bounds
         */
        template<typename T>
        bool isType(size_t index) const {
            if (index >= GetArraySize()) return false;
            return m_data[index]->is<T>();
        }
    };
}

