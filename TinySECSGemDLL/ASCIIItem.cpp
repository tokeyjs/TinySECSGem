#include "ASCIIItem.h"

using namespace TinySECSGem;

ASCIIItem::ASCIIItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_ASCII;
}

ASCIIItem::ASCIIItem(char val)
    : BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_ASCII;
    push_back(val);
}

ASCIIItem::ASCIIItem(char* start, char* end)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_ASCII;
    m_data = std::string(start, end);
}

ASCIIItem::ASCIIItem(const char* str)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_ASCII;
    m_data = str;
}

ASCIIItem::ASCIIItem(std::string str)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_ASCII;
    m_data = str;
}


ASCIIItem::~ASCIIItem()
{
}

BaseItem* ASCIIItem::clone()
{
    ASCIIItem* item = new ASCIIItem();
    item->m_data = m_data;
    return item;
}

size_t ASCIIItem::totalByteSize() const
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

int ASCIIItem::Serialization(BYTE* buff, size_t buffSize)
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

ASCIIItem* ASCIIItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_ASCII)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(char) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(char);
    return new ASCIIItem((char*)bufStart + index, (char*)bufStart + index + length);
}

std::string ASCIIItem::print(std::string frontStr/* = ""*/) const
{
    // "< A[2] "13" >"
    std::string formatStr = frontStr + "< A";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] \"");
    formatStr += (m_data + "\" ");
    formatStr += ">";
    return formatStr;
}

size_t ASCIIItem::GetArraySize() const
{
    return m_data.size();
}

void ASCIIItem::clear()
{
    m_data.clear();
}


void ASCIIItem::push_back(char val)
{
    m_data.push_back(val);
}

void ASCIIItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void ASCIIItem::insert(size_t pos, char val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void ASCIIItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}

void ASCIIItem::set(const char* str)
{
    m_data = str;
}

void ASCIIItem::set(std::string str)
{
    m_data = str;
}

std::string& ASCIIItem::get()
{
    return m_data;
}

char& ASCIIItem::operator[](size_t index) {
    return m_data[index];
}

ASCIIItem& ASCIIItem::operator=(const ASCIIItem& item) {
    if (this != &item) {
        m_data = item.m_data;
    }
    return *this;
}
ASCIIItem& ASCIIItem::operator=(const std::string& str) {
    m_data = str;
    return *this;
}
ASCIIItem& ASCIIItem::operator=(const char* str) {
    m_data = str;
    return *this;
}

ASCIIItem ASCIIItem::operator+(const ASCIIItem& item) const {
    return ASCIIItem(m_data + item.m_data);
}
ASCIIItem ASCIIItem::operator+(const std::string& str) const {
    return ASCIIItem(m_data + str);
}
ASCIIItem ASCIIItem::operator+(const char* str) const {
    return ASCIIItem(m_data + str);
}

ASCIIItem& ASCIIItem::operator+=(const ASCIIItem& item) {
    m_data += item.m_data;
    return *this;
}
ASCIIItem& ASCIIItem::operator+=(const std::string& str) {
    m_data += str;
    return *this;
}
ASCIIItem& ASCIIItem::operator+=(const char* str) {
    m_data += str;
    return *this;
}
