#pragma once
#include "BaseItem.h"
#include <string>


namespace TinySECSGem {

    /**
    * @brief SECS-II ASCII string data item implementation
    *
    * Represents the ASCII data type (format code 'A') in the SECS/GEM protocol.
    * Stores character data using std::string. Supports character append,
    * insert, delete, string concatenation, serialization, and deserialization.
    *
    * ASCIIItem is a leaf SECS-II data node. It can be used directly as
    * message body data or nested inside ListItem/TopItem.
    */
    class TinySECSGEM_API ASCIIItem : public BaseItem
    {
    private:
        std::string m_data;   ///< ASCII string data

    public:

        /**
         * @brief Construct an empty ASCII item
         */
        ASCIIItem();

        /**
         * @brief Construct a single-character ASCII item
         *
         * @param val Initial character
         */
        ASCIIItem(char val);

        /**
         * @brief Construct an ASCII item from a character range (end exclusive)
         *
         * @param start Start character pointer
         * @param end End character pointer (exclusive)
         */
        ASCIIItem(char* start, char* end);

        /**
         * @brief Construct an ASCII item from a C string
         *
         * @param str Null-terminated string
         */
        ASCIIItem(const char* str);

        /**
         * @brief Construct an ASCII item from std::string
         *
         * @param str Initial string
         */
        ASCIIItem(std::string str);

        /**
         * @brief Destroy the ASCII item
         */
        ~ASCIIItem();

        /**
         * @brief Clone this ASCII item
         *
         * @return Newly created ASCIIItem (caller must free)
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
         * @return ASCIIItem* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static ASCIIItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print content (debug)
         *
         * @param frontStr Indentation prefix
         * @return std::string Formatted string
         */
        virtual std::string print(std::string frontStr = "") const;

        /**
         * @brief Get the string length
         *
         * @return Character count
         */
        virtual size_t GetArraySize() const;

        /**
         * @brief Clear the string content
         */
        virtual void clear();

        /**
         * @brief Append a character at the end
         *
         * @param val Character to append
         */
        void push_back(char val);

        /**
         * @brief Remove the last character
         */
        void pop_back();

        /**
         * @brief Insert a character at the specified position
         *
         * @param pos Insertion position
         * @param val Character to insert
         */
        void insert(size_t pos, char val);

        /**
         * @brief Erase a character at the specified position
         *
         * @param pos Position to erase
         */
        void erase(size_t pos);

        /**
         * @brief Replace content with a C string
         *
         * @param str New string content
         */
        void set(const char* str);

        /**
         * @brief Replace content with std::string
         *
         * @param str New string content
         */
        void set(std::string str);

        /**
         * @brief Get a reference to the internal string
         *
         * @return String reference
         */
        std::string& get();

        /**
         * @brief Access character at index (unchecked)
         *
         * @param index Character index
         * @return Reference to the character
         */
        char& operator[](size_t index);

        /**
         * @brief Copy assignment
         *
         * @param item Source ASCIIItem
         * @return Reference to this
         */
        ASCIIItem& operator=(const ASCIIItem& item);

        /**
         * @brief Assign from std::string
         *
         * @param str String content
         * @return Reference to this
         */
        ASCIIItem& operator=(const std::string& str);

        /**
         * @brief Assign from C string
         *
         * @param str C string content
         * @return Reference to this
         */
        ASCIIItem& operator=(const char* str);

        /**
         * @brief Concatenate with another ASCIIItem (new object)
         *
         * @param item Target ASCIIItem
         * @return New concatenated object
         */
        ASCIIItem operator+(const ASCIIItem& item) const;

        /**
         * @brief Concatenate with std::string (new object)
         *
         * @param str Target string
         * @return New concatenated object
         */
        ASCIIItem operator+(const std::string& str) const;

        /**
         * @brief Concatenate with C string (new object)
         *
         * @param str Target string
         * @return New concatenated object
         */
        ASCIIItem operator+(const char* str) const;

        /**
         * @brief In-place append ASCIIItem
         *
         * @param item Target ASCIIItem
         * @return Reference to this
         */
        ASCIIItem& operator+=(const ASCIIItem& item);

        /**
         * @brief In-place append std::string
         *
         * @param str Target string
         * @return Reference to this
         */
        ASCIIItem& operator+=(const std::string& str);

        /**
         * @brief In-place append C string
         *
         * @param str Target string
         * @return Reference to this
         */
        ASCIIItem& operator+=(const char* str);
    };
}

