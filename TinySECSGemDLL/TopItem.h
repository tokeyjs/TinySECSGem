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
#include "ListItem.h"

// TopItem is the physical root node used for all parsed SECS-II message trees.
// It is not a real SECS-II type (E5) — it only exists to wrap the top-level
// data structure for uniform handling.
// Most TopItem instances contain a single child node, but multiple siblings
// are possible when the message body has parallel items.
//
//   Example (1): a single LIST root
//     < L[2]
//         < B[1] 12 >
//         < A[3] "erq" >
//     >
//   Tree:
//     TopItem
//         |-ListItem
//             |-BinaryItem
//             |-ASCIIItem
//
//   Example (2): multiple parallel items
//     < L[2]
//         < B[1] 12 >
//         < A[3] "erq" >
//     >
//     < A[3] "rrr" >
//     < L[1]
//         < L[2]
//             < U4[1] 123 >
//             < A[2] "ww" >
//         >
//     >
//   Tree:
//     TopItem
//         |-ListItem
//             |-BinaryItem
//             |-ASCIIItem
//         |-ASCIIItme
//         |-ListItem
//             |-ListItem
//                 |-UInt4ByteItem
//                 |-ASCIIItem

namespace TinySECSGem {


    /**
    * @brief SECS-II message tree virtual root node (TopItem)
    *
    * Acts as the logical root node for parsed SECS/GEM messages.
    * Provides a uniform container for organizing the complete SECS-II
    * item tree.
    *
    * @note TopItem does NOT correspond to a real SECS-II type (E5).
    *       It is a virtual root for organizing the item tree structure.
    *       Every received SECS message with a body uses TopItem as the
    *       top-level wrapper. When sending a message with multiple
    *       top-level items, TopItem can be used to wrap them.
    *
    * All received messages with a body have TopItem, e.g.:
    *
    *   (1) <L[2]
    *          <U4[1] 1 >
    *          <U4[1] 2 >
    *        >
    *   Tree:
    *     TopItem
    *       -ListItem
    *           -UInt4ByteItem
    *           -UInt4ByteItem
    *
    *   (2)
    *       <U1[1] 1 >
    *       <U4[1] 2 >
    *   Tree:
    *     TopItem
    *       -UInt1ByteItem
    *       -UInt4ByteItem
    */
    class TinySECSGEM_API TopItem : public BaseItem
    {
    private:
        std::vector<BaseItem*> m_data;   ///< Child node list (SECS-II data tree container)

    public:

        /**
         * @brief Construct an empty TopItem
         */
        TopItem();

        /**
         * @brief Construct a single-node TopItem
         *
         * @param val Initial child pointer
         * @note TopItem takes ownership of the pointer
         */
        TopItem(BaseItem* val);

        /**
         * @brief Copy constructor (deep copy of the entire tree)
         *
         * @param item Source TopItem
         */
        TopItem(const TopItem& item);

        /**
         * @brief Copy assignment (deep copy)
         *
         * @param item Source TopItem
         * @return Reference to this object
         */
        TopItem& operator=(const TopItem& item);

        /**
         * @brief Assign from another TopItem pointer (deep copies children)
         *
         * @param val Source TopItem pointer
         * @return Reference to this object
         *
         * @note Clears existing children then deep-copies val's children.
         */
        TopItem& operator=(TopItem* val);

        /**
         * @brief Destructor, releases all child nodes
         */
        ~TopItem();

        /**
         * @brief Clone this TopItem (deep copy of the entire tree)
         *
         * @return Newly created copy (caller owns the memory)
         */
        virtual BaseItem* clone();

        /**
         * @brief Compute serialized byte size for the entire tree
         *
         * @return size_t Total bytes including all children
         */
        virtual size_t totalByteSize() const;

        /**
         * @brief Serialize the entire SECS-II message tree
         *
         * @param buff Output buffer
         * @param buffSize Maximum buffer length
         * @return int Bytes written, or -1 on failure
         */
        virtual int Serialization(BYTE* buff, size_t buffSize);

        /**
         * @brief Deserialize a TopItem tree from byte stream
         *
         * @param bufStart Start buffer pointer
         * @param bufEnd End buffer pointer (exclusive)
         * @return TopItem* Parsed tree root (nullptr on failure)
         *
         * @note Caller owns the returned object
         */
        static TopItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print the SECS-II tree structure (debug)
         *
         * @param frontStr Indentation prefix
         * @return std::string Readable tree string
         */
        virtual std::string print(std::string frontStr = "") const;

        /**
         * @brief Get the number of child nodes
         *
         * @return size_t Child count
         */
        virtual size_t GetArraySize() const;

        /**
         * @brief Clear the entire tree and release all children
         */
        virtual void clear();

        /**
         * @brief Append a child node at the end
         *
         * @param val Child pointer (TopItem takes ownership)
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
         *
         * @param pos Insertion index (0 ~ size)
         * @param val Node to insert (TopItem takes ownership)
         */
        void insert(size_t pos, BaseItem* val);

        /**
         * @brief Erase the child at the specified position
         *
         * @param pos Index to erase
         */
        void erase(size_t pos);

        /**
         * @brief Get child at index and cast to target type
         *
         * @tparam T Target type
         * @param index Node index
         * @return T* Cast pointer on success, nullptr on failure
         */
        template<typename T>
        T* get(size_t index) const {
            if (index >= GetArraySize()) return nullptr;
            return m_data[index]->as<T>();
        }

        /**
         * @brief Check whether the child at index is of the specified type
         *
         * @tparam T Target type
         * @param index Node index
         * @return true if type matches
         * @return false if mismatch or out of bounds
         */
        template<typename T>
        bool isType(size_t index) const {
            if (index >= GetArraySize()) return false;
            return m_data[index]->is<T>();
        }
    };

}


