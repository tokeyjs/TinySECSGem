#pragma once
#include "BaseItem.h"


namespace TinySECSGem {

    /**
    * @brief SECS-II JIS8 string data item implementation (not yet supported)
    *
    * Represents the JIS-8 string format (JIS8) in the SECS/GEM protocol.
    * Stores character data using std::string.
    *
    * @note The current TinySECSGem library does NOT implement JIS encoding
    *       conversion or display. This type exists only for protocol format
    *       compatibility. Use ASCIIItem for string communication instead.
    */
    class TinySECSGEM_API JIS8Item : public BaseItem
    {
    private:
        std::string m_data;   ///< JIS8 string data buffer

    public:

        /**
         * @brief Construct an empty JIS8 item
         */
        JIS8Item();

        /**
         * @brief Construct a single-character JIS8 item
         *
         * @param val Initial character
         */
        JIS8Item(char val);

        /**
         * @brief Construct from a character range (end exclusive)
         *
         * @param start Start character pointer
         * @param end End character pointer (exclusive)
         */
        JIS8Item(char* start, char* end);

        /**
         * @brief Construct from a C string
         *
         * @param str String content
         */
        JIS8Item(const char* str);

        /**
         * @brief Construct from std::string
         *
         * @param str String content
         */
        JIS8Item(std::string str);

        /**
         * @brief Destroy the JIS8 item
         */
        ~JIS8Item();

        /**
         * @brief Clone this JIS8 item
         *
         * @return Newly created JIS8Item (caller must free)
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
         * @return JIS8Item* Parsed item, nullptr on failure
         *
         * @note Caller owns the returned object
         */
        static JIS8Item* Deserialization(BYTE* bufStart, BYTE* bufEnd);

        /**
         * @brief Print content (debug)
         *
         * @param frontStr Indentation prefix
         * @return Formatted string
         */
        virtual std::string print(std::string frontStr = "") const;

        /**
         * @brief Check whether the string is empty
         *
         * @return true if empty
         */
        virtual bool isEmpty() const;

        /**
         * @brief Get the character count
         *
         * @return String length
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
         *
         * @warning Out-of-bounds access has undefined behavior
         */
        char& operator[](size_t index);

        /**
         * @brief Copy assignment
         *
         * @param item Source JIS8Item
         * @return Reference to this
         */
        JIS8Item& operator=(const JIS8Item& item);

        /**
         * @brief Assign from std::string
         *
         * @param str String content
         * @return Reference to this
         */
        JIS8Item& operator=(const std::string& str);

        /**
         * @brief Assign from C string
         *
         * @param str String content
         * @return Reference to this
         */
        JIS8Item& operator=(const char* str);

        /**
         * @brief Concatenate with another JIS8Item (new object)
         *
         * @param item Target JIS8Item
         * @return New concatenated object
         */
        JIS8Item operator+(const JIS8Item& item) const;

        /**
         * @brief Concatenate with std::string (new object)
         *
         * @param str Target string
         * @return New concatenated object
         */
        JIS8Item operator+(const std::string& str) const;

        /**
         * @brief Concatenate with C string (new object)
         *
         * @param str Target string
         * @return New concatenated object
         */
        JIS8Item operator+(const char* str) const;

        /**
         * @brief In-place append JIS8Item
         *
         * @param item Object to append
         * @return Reference to this
         */
        JIS8Item& operator+=(const JIS8Item& item);

        /**
         * @brief In-place append std::string
         *
         * @param str String to append
         * @return Reference to this
         */
        JIS8Item& operator+=(const std::string& str);

        /**
         * @brief In-place append C string
         *
         * @param str String to append
         * @return Reference to this
         */
        JIS8Item& operator+=(const char* str);
    };

}


