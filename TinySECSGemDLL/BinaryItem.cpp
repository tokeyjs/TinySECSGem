#include "BinaryItem.h"

using namespace TinySECSGem;


BinaryItem::BinaryItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_BINARY;
}

BinaryItem::BinaryItem(BYTE val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_BINARY;
    push_back(val);
}

BinaryItem::BinaryItem(BYTE* start, BYTE* end)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_BINARY;
    if (start < end)
        m_data = std::vector<BYTE>(start, end);
}

BinaryItem::BinaryItem(const BinaryItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_BINARY;
    m_data = item.m_data;
}

BinaryItem& BinaryItem::operator=(const BinaryItem& item)
{
    m_data = item.m_data;
    return *this;
}

BinaryItem& BinaryItem::operator=(BYTE val)
{
    clear();
    push_back(val);
    return *this;
}

BinaryItem::~BinaryItem()
{
}

BaseItem* BinaryItem::clone()
{
    BinaryItem* item = new BinaryItem();
    item->m_data = m_data;
    return item;
}

size_t BinaryItem::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(BYTE);
    if (GetArraySize() * sizeof(BYTE) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(BYTE) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(BYTE) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}

int BinaryItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(BYTE), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        buff[index++] = (*this)[i];
    }
    return totalByteSize();
}

BinaryItem* BinaryItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_BINARY)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(BYTE) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; 
    length = lengthByte / sizeof(BYTE);
    return new BinaryItem(bufStart + index, bufStart + index + length);
}

std::string BinaryItem::print(std::string frontStr/* = ""*/) const
{
    // "< B[2] 12 3 >"
    std::string formatStr = frontStr + "< B";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] ");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += (std::to_string(m_data[i]) + " ");
    formatStr += ">";
    return formatStr;
}

size_t BinaryItem::GetArraySize() const
{
    return m_data.size();
}

void BinaryItem::clear()
{
    m_data.clear();
}

void BinaryItem::push_back(BYTE val)
{
    m_data.push_back(val);
}

void BinaryItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void BinaryItem::insert(size_t pos, BYTE val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void BinaryItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}

void BinaryItem::set(BYTE* start, BYTE* end)
{
    m_data = std::vector<BYTE>(start, end);
}

void BinaryItem::set(BYTE val, size_t index/* = 0*/)
{
    if (index >= GetArraySize())
    {
        push_back(val);
    }
    else
    {
        m_data[index] = val;
    }
}

BYTE BinaryItem::get(size_t index /*= 0*/)
{
    if (GetArraySize() == 0)
        return 0;
    if (index >= GetArraySize())
        index = GetArraySize() - 1;
    return m_data[index];
}

BYTE& BinaryItem::operator[](size_t index) {
    return m_data[index];
}
