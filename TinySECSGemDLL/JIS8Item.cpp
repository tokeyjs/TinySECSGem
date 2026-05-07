#include "JIS8Item.h"

using namespace TinySECSGem;

JIS8Item::JIS8Item()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_JIS;
}

JIS8Item::JIS8Item(char val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_JIS;
    push_back(val);
}

JIS8Item::JIS8Item(char* start, char* end)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_JIS;
    m_data = std::string(start, end);
}

JIS8Item::JIS8Item(const char* str)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_JIS;
    m_data = str;
}

JIS8Item::JIS8Item(std::string str)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_JIS;
    m_data = str;
}


JIS8Item::~JIS8Item()
{
}

BaseItem* JIS8Item::clone()
{
    JIS8Item* item = new JIS8Item();
    item->m_data = m_data;
    return item;
}

size_t JIS8Item::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(char);
    if (GetArraySize() * sizeof(char) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(char) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(char) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}

int JIS8Item::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(char), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        buff[index++] = (*this)[i];
    }
    return totalByteSize();
}

JIS8Item* JIS8Item::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_JIS)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(char) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(char);
    return new JIS8Item((char*)bufStart + index, (char*)bufStart + index + length);
}

std::string JIS8Item::print(std::string frontStr /*= ""*/) const
{
    // "< J[2] "12" >"
    std::string formatStr = frontStr + "< J";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] \"");
    formatStr += (m_data + "\" ");
    formatStr += ">";
    return formatStr;
}

bool JIS8Item::isEmpty() const
{
    return m_data.empty();
}

size_t JIS8Item::GetArraySize() const
{
    return m_data.size();
}

void JIS8Item::clear()
{
    m_data.clear();
}

void JIS8Item::push_back(char val)
{
    m_data.push_back(val);
}

void JIS8Item::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

char& JIS8Item::back()
{
    return m_data.back();
}

char& JIS8Item::front()
{
    return m_data.front();
}

void JIS8Item::insert(size_t pos, char val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void JIS8Item::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}

void JIS8Item::set(const char* str)
{
    m_data = str;
}

void JIS8Item::set(std::string str)
{
    m_data = str;
}

std::string& JIS8Item::get()
{
    return m_data;
}

char& JIS8Item::operator[](size_t index) {
    return m_data[index];
}

JIS8Item& JIS8Item::operator=(const JIS8Item& item) {
    if (this != &item) {
        m_data = item.m_data;
    }
    return *this;
}
JIS8Item& JIS8Item::operator=(const std::string& str) {
    m_data = str;
    return *this;
}
JIS8Item& JIS8Item::operator=(const char* str) {
    m_data = str;
    return *this;
}

JIS8Item JIS8Item::operator+(const JIS8Item& item) const {
    return JIS8Item(m_data + item.m_data);
}
JIS8Item JIS8Item::operator+(const std::string& str) const {
    return JIS8Item(m_data + str);
}
JIS8Item JIS8Item::operator+(const char* str) const {
    return JIS8Item(m_data + str);
}

JIS8Item& JIS8Item::operator+=(const JIS8Item& item) {
    m_data += item.m_data;
    return *this;
}
JIS8Item& JIS8Item::operator+=(const std::string& str) {
    m_data += str;
    return *this;
}
JIS8Item& JIS8Item::operator+=(const char* str) {
    m_data += str;
    return *this;
}
