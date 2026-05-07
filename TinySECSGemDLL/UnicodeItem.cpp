#include "UnicodeItem.h"

using namespace TinySECSGem;

UnicodeItem::UnicodeItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_UNICODE;
}

UnicodeItem::UnicodeItem(char val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_UNICODE;
    push_back(val);
}

UnicodeItem::UnicodeItem(char* start, char* end)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_UNICODE;
    m_data = std::string(start, end);
}

UnicodeItem::UnicodeItem(const char* str)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_UNICODE;
    m_data = str;
}

UnicodeItem::UnicodeItem(std::string str)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_UNICODE;
    m_data = str;
}

UnicodeItem::~UnicodeItem()
{
}

BaseItem* UnicodeItem::clone()
{
    UnicodeItem* item = new UnicodeItem();
    item->m_data = m_data;
    return item;
}

size_t UnicodeItem::totalByteSize() const
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

int UnicodeItem::Serialization(BYTE* buff, size_t buffSize)
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

UnicodeItem* UnicodeItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_UNICODE)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(char) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(char);
    return new UnicodeItem((char*)bufStart + index, (char*)bufStart + index + length);
}

std::string UnicodeItem::print(std::string frontStr /*= ""*/) const
{
    // "< A "12" >"
    std::string formatStr = frontStr + "< U";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] \"");
    formatStr += (m_data + "\" ");
    formatStr += ">";
    return formatStr;
}

size_t UnicodeItem::GetArraySize() const
{
    return m_data.size();
}

void UnicodeItem::clear()
{
    m_data.clear();
}

void UnicodeItem::push_back(char val)
{
    m_data.push_back(val);
}

void UnicodeItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

char& UnicodeItem::back()
{
    return m_data.back();
}

char& UnicodeItem::front()
{
    return m_data.front();
}

void UnicodeItem::insert(size_t pos, char val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void UnicodeItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}

void UnicodeItem::set(const char* str)
{
    m_data = str;
}

void UnicodeItem::set(std::string str)
{
    m_data = str;
}

std::string& UnicodeItem::get()
{
    return m_data;
}

char& UnicodeItem::operator[](size_t index) {
    return m_data[index];
}

UnicodeItem& UnicodeItem::operator=(const UnicodeItem& item) {
    if (this != &item) {
        m_data = item.m_data;
    }
    return *this;
}
UnicodeItem& UnicodeItem::operator=(const std::string& str) {
    m_data = str;
    return *this;
}
UnicodeItem& UnicodeItem::operator=(const char* str) {
    m_data = str;
    return *this;
}

UnicodeItem UnicodeItem::operator+(const UnicodeItem& item) const {
    return UnicodeItem(m_data + item.m_data);
}
UnicodeItem UnicodeItem::operator+(const std::string& str) const {
    return UnicodeItem(m_data + str);
}
UnicodeItem UnicodeItem::operator+(const char* str) const {
    return UnicodeItem(m_data + str);
}

UnicodeItem& UnicodeItem::operator+=(const UnicodeItem& item) {
    m_data += item.m_data;
    return *this;
}
UnicodeItem& UnicodeItem::operator+=(const std::string& str) {
    m_data += str;
    return *this;
}
UnicodeItem& UnicodeItem::operator+=(const char* str) {
    m_data += str;
    return *this;
}

