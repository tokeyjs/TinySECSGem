#pragma once
#include "BaseItem.h"
#include <string>


namespace TinySECSGem {

    /**
    * @brief SECS-II Unicode string data item implementation (NOT supported — do not use)
    *
    * Represents the UNICODE string format in the SECS/GEM protocol.
    * Stores raw byte data using std::string.
    *
    * @warning The current TinySECSGem library does NOT implement any Unicode
    *          encoding conversion or parsing. m_data is treated as raw bytes
    *          without character set semantics. This type is a protocol
    *          placeholder only — using it will produce garbled data.
    *
    *          For string communication, use ASCIIItem instead.
    */
    class TinySECSGEM_API UnicodeItem : public BaseItem
    {
    private:
        std::string m_data;   ///< Raw byte data (no Unicode parsing)

    public:

        /**
         * @brief Construct an empty Unicode item
         */
        UnicodeItem();

        /**
         * @brief Construct a single-character Unicode item
         *
         * @param val Initial character
         */
        UnicodeItem(char val);

        /**
         * @brief Construct from a character range (end exclusive)
         *
         * @param start Start character pointer
         * @param end End character pointer (exclusive)
         */
        UnicodeItem(char* start, char* end);

        /**
         * @brief Construct from a C string
         *
         * @param str String content
         */
        UnicodeItem(const char* str);

        /**
         * @brief Construct from std::string
         *
         * @param str String content
         */
        UnicodeItem(std::string str);

        /**
         * @brief Destroy the Unicode item
         */
        ~UnicodeItem();

        /**
         * @brief Clone this Unicode item
         *
         * @return Newly created UnicodeItem (caller must free)
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
         * @return UnicodeItem* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static UnicodeItem* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print raw content (debug)
         *
         * @param frontStr Indentation prefix
         * @return Raw string content (character correctness not guaranteed)
         */
        virtual std::string print(std::string frontStr = "") const;

        /**
         * @brief Get the character count
         *
         * @return Character count
         */
        virtual size_t GetArraySize() const;

        /**
         * @brief Clear data content
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
         * @brief Get a reference to the last character
         *
         * @return Last character
         */
        char& back();

        /**
         * @brief Get a reference to the first character
         *
         * @return First character
         */
        char& front();

        /**
         * @brief Insert a character at the specified position
         *
         * @param pos Insertion index
         * @param val Character to insert
         */
        void insert(size_t pos, char val);

        /**
         * @brief Erase the character at the specified position
         *
         * @param pos Index to erase
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
         * @brief Get a reference to the internal raw string
         *
         * @return Byte data reference
         */
        std::string& get();

        /**
         * @brief Access character at index (unchecked)
         *
         * @param index Character index
         * @return Reference to the character
         *
         * @warning Out-of-bounds access has undefined behavior
         */
        char& operator[](size_t index);

        /**
         * @brief Copy assignment
         *
         * @param item Source UnicodeItem
         * @return Reference to this
         */
        UnicodeItem& operator=(const UnicodeItem& item);

        /**
         * @brief Assign from std::string
         *
         * @param str String content
         * @return Reference to this
         */
        UnicodeItem& operator=(const std::string& str);

        /**
         * @brief Assign from C string
         *
         * @param str String content
         * @return Reference to this
         */
        UnicodeItem& operator=(const char* str);

        /**
         * @brief Concatenate with another UnicodeItem (new object)
         *
         * @param item Target object
         * @return Concatenated result
         */
        UnicodeItem operator+(const UnicodeItem& item) const;

        /**
         * @brief Concatenate with std::string (new object)
         *
         * @param str Target string
         * @return Concatenated result
         */
        UnicodeItem operator+(const std::string& str) const;

        /**
         * @brief Concatenate with C string (new object)
         *
         * @param str Target string
         * @return Concatenated result
         */
        UnicodeItem operator+(const char* str) const;

        /**
         * @brief In-place append UnicodeItem
         *
         * @param item Object to append
         * @return Reference to this
         */
        UnicodeItem& operator+=(const UnicodeItem& item);

        /**
         * @brief In-place append std::string
         *
         * @param str String to append
         * @return Reference to this
         */
        UnicodeItem& operator+=(const std::string& str);

        /**
         * @brief In-place append C string
         *
         * @param str String to append
         * @return Reference to this
         */
        UnicodeItem& operator+=(const char* str);
    };

}
