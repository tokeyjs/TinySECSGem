#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "TinySECSGem.h"

namespace TinySECSGem
{
    class BinaryItem;
    class BooleanItem;
    class ASCIIItem;
    class UnicodeItem;
    class JIS8Item;
    class Int1ByteItem;
    class Int2ByteItem;
    class Int4ByteItem;
    class Int8ByteItem;
    class UInt1ByteItem;
    class UInt2ByteItem;
    class UInt4ByteItem;
    class UInt8ByteItem;
    class Float4ByteItem;
    class Float8ByteItem;
    class ListItem;


    /**
    * @brief SECS-II data item abstract base class
    *
    * Defines the unified interface for all SECS-II data types in the
    * SECS/GEM protocol. Each data node in a SECS message tree inherits
    * from this class, including leaf types (ASCII, U1, U2, etc.) and
    * the composite LIST type.
    */
    class TinySECSGEM_API BaseItem
    {
    protected:
        EnumSECSItemType m_eItemType;   ///< SECS-II item type identifier (ASCII/U1/U2/LIST, etc.)

    public:

        /**
         * @brief Construct a BaseItem
         */
        BaseItem();

        /**
         * @brief Destroy a BaseItem
         */
        virtual ~BaseItem();

        /**
         * @brief Get the total serialized byte size of this item (including children)
         * @return Byte count occupied by this item in serialized form
         */
        virtual size_t totalByteSize() const = 0;

        /**
         * @brief Get the number of array elements
         * @return Element count
         */
        virtual size_t GetArraySize() const = 0;

        /**
         * @brief Clear all data in this item
         */
        virtual void clear() = 0;

        /**
         * @brief Deep-copy this item
         * @return Newly cloned object (caller must free the memory)
         */
        virtual BaseItem* clone() = 0;

        /**
         * @brief Convert the data structure to a readable string (debug)
         * @param frontStr Output prefix (for indentation formatting)
         * @return Formatted string representation
         */
        virtual std::string print(std::string frontStr = "") const = 0;

        /**
         * @brief Serialize this item to SECS-II byte stream
         * @param buff Output buffer
         * @param buffSize Output buffer size
         * @return Bytes written, or -1 on failure
         */
        virtual int Serialization(BYTE* buff, size_t buffSize) = 0;

        /**
         * @brief Deserialize a BaseItem from SECS-II byte stream
         * @param startBufPos Start buffer pointer
         * @param endBufPos End buffer pointer (exclusive)
         * @param bOk Output flag indicating success or failure
         * @return Deserialized BaseItem pointer (nullptr on failure)
         */
        static BaseItem* Deserialization(BYTE* startBufPos, BYTE* endBufPos, bool& bOk);

        /**
         * @brief Parse the SECS-II item format code and length from the item header
         *
         * Extracts the type format code and encoded length information from
         * the SECS-II Item Header.
         *
         * @param bufStart Start buffer pointer
         * @param bufEnd End buffer pointer (exclusive)
         * @param format Output: item type format code
         * @param lengthByteSize Output: number of bytes used for the length field
         * @param lengthByte Output: data length value
         * @return true on success, false on failure
         */
        static bool DeserializationItemHeader(
            BYTE* bufStart,
            BYTE* bufEnd,
            EnumSECSItemType& format,
            size_t& lengthByteSize,
            size_t& lengthByte);

        /**
         * @brief Serialize the SECS-II item format code and length header
         *
         * Encodes the type format and length information into SECS-II
         * Item Header format.
         *
         * @param bufStart Output buffer start position
         * @param bufEnd Output buffer end position (exclusive)
         * @param format Item type format code
         * @param singleTypeSize Size of each array element
         * @param arraySize Number of array elements
         * @return Bytes written, or -1 on failure
         */
        static int SerializationItemHeader(
            BYTE* bufStart,
            BYTE* bufEnd,
            EnumSECSItemType format,
            size_t singleTypeSize,
            size_t arraySize);

        /**
         * @brief Get the item type format code
         * @return EnumSECSItemType Current SECS-II item type
         */
        EnumSECSItemType getFormat() const;

        /**
         * @brief Type-safe downcast to the specified type
         * @tparam T Target type
         * @return Pointer to the target type, or nullptr on failure
         */
        template<typename T>
        T* as() {
            return dynamic_cast<T*>(this);
        }

        /**
         * @brief Const version of type-safe downcast
         */
        template<typename T>
        const T* as() const {
            return dynamic_cast<const T*>(this);
        }

        /**
         * @brief Check whether the current item is of the specified type
         * @tparam T Target type
         * @return true if types match
         * @return false if types do not match
         */
        template<typename T>
        bool is() const {
            return dynamic_cast<const T*>(this) != nullptr;
        }
    };

}
