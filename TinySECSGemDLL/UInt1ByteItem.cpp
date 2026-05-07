#include "UInt1ByteItem.h"

using namespace TinySECSGem;


UInt1ByteItem::UInt1ByteItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_UINT1;
}

UInt1ByteItem::UInt1ByteItem(uint8_t val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_UINT1;
    push_back(val);
}


UInt1ByteItem::UInt1ByteItem(const UInt1ByteItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_UINT1;
    m_data = item.m_data;
}

UInt1ByteItem& UInt1ByteItem::operator=(const UInt1ByteItem& item)
{
    m_data = item.m_data;
    return *this;
}

UInt1ByteItem& UInt1ByteItem::operator=(uint8_t val)
{
    clear();
    push_back(val);
    return *this;
}

UInt1ByteItem::~UInt1ByteItem()
{
}

BaseItem* UInt1ByteItem::clone()
{
    UInt1ByteItem* item = new UInt1ByteItem();
    item->m_data = m_data;
    return item;
}

uint8_t& UInt1ByteItem::operator[](size_t index) {
    return m_data[index];
}


size_t UInt1ByteItem::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(uint8_t);
    if (GetArraySize() * sizeof(uint8_t) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(uint8_t) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(uint8_t) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}

int UInt1ByteItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(uint8_t), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        buff[index++] = (*this)[i];
    }
    return totalByteSize();
}

UInt1ByteItem* UInt1ByteItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_UINT1)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(uint8_t) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(uint8_t);
    UInt1ByteItem* item = new UInt1ByteItem();
    for (int i = 0; i < length; i++)
    {
        uint8_t val = bufStart[index++];
        item->push_back(val);
    }
    return item;
}

std::string UInt1ByteItem::print(std::string frontStr /*= ""*/) const
{
    // "< U1[2] 12 3 >"
    std::string formatStr = frontStr + "< U1";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] ");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += (std::to_string(m_data[i]) + " ");
    formatStr += ">";
    return formatStr;
}

size_t UInt1ByteItem::GetArraySize() const
{
    return m_data.size();
}

void UInt1ByteItem::clear()
{
    m_data.clear();
}

void UInt1ByteItem::push_back(uint8_t val)
{
    m_data.push_back(val);
}

void UInt1ByteItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void UInt1ByteItem::set(uint8_t val, size_t index /*= 0*/)
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

uint8_t UInt1ByteItem::get(size_t index /*= 0*/)
{
    if (GetArraySize() == 0)
        return 0;
    if (index >= GetArraySize())
        index = GetArraySize() - 1;
    return m_data[index];
}

void UInt1ByteItem::insert(size_t pos, uint8_t val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void UInt1ByteItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}
