#include "UInt8ByteItem.h"

using namespace TinySECSGem;


UInt8ByteItem::UInt8ByteItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_UINT8;
}

UInt8ByteItem::UInt8ByteItem(uint64_t val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_UINT8;
    push_back(val);
}

UInt8ByteItem::UInt8ByteItem(const UInt8ByteItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_UINT8;
    m_data = item.m_data;
}

UInt8ByteItem& UInt8ByteItem::operator=(const UInt8ByteItem& item)
{
    m_data = item.m_data;
    return *this;
}

UInt8ByteItem& UInt8ByteItem::operator=(uint64_t val)
{
    clear();
    push_back(val);
    return *this;
}

UInt8ByteItem::~UInt8ByteItem()
{
}

BaseItem* UInt8ByteItem::clone()
{
    UInt8ByteItem* item = new UInt8ByteItem();
    item->m_data = m_data;
    return item;
}

uint64_t& UInt8ByteItem::operator[](size_t index) {
    return m_data[index];
}

size_t UInt8ByteItem::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(uint64_t);
    if (GetArraySize() * sizeof(uint64_t) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(uint64_t) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(uint64_t) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}

int UInt8ByteItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(uint64_t), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        uint64_t val = (*this)[i];
        unsigned char* valPtr = (unsigned char*)&val;
#if SECS_ENDIAN == SECS_LITTLE_ENDIAN
        buff[index++] = valPtr[7];
        buff[index++] = valPtr[6];
        buff[index++] = valPtr[5];
        buff[index++] = valPtr[4];
        buff[index++] = valPtr[3];
        buff[index++] = valPtr[2];
        buff[index++] = valPtr[1];
        buff[index++] = valPtr[0];
#else
        buff[index++] = valPtr[0];
        buff[index++] = valPtr[1];
        buff[index++] = valPtr[2];
        buff[index++] = valPtr[3];
        buff[index++] = valPtr[4];
        buff[index++] = valPtr[5];
        buff[index++] = valPtr[6];
        buff[index++] = valPtr[7];
#endif
    }
    return totalByteSize();
}

UInt8ByteItem* UInt8ByteItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_UINT8)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(uint64_t) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(uint64_t);
    UInt8ByteItem* item = new UInt8ByteItem();
    for (int i = 0; i < length; i++)
    {
        uint64_t val = 0;
        unsigned char* valPtr = (unsigned char*)&val;
#if SECS_ENDIAN == SECS_LITTLE_ENDIAN
        valPtr[7] = bufStart[index++];
        valPtr[6] = bufStart[index++];
        valPtr[5] = bufStart[index++];
        valPtr[4] = bufStart[index++];
        valPtr[3] = bufStart[index++];
        valPtr[2] = bufStart[index++];
        valPtr[1] = bufStart[index++];
        valPtr[0] = bufStart[index++];
#else
        valPtr[0] = bufStart[index++];
        valPtr[1] = bufStart[index++];
        valPtr[2] = bufStart[index++];
        valPtr[3] = bufStart[index++];
        valPtr[4] = bufStart[index++];
        valPtr[5] = bufStart[index++];
        valPtr[6] = bufStart[index++];
        valPtr[7] = bufStart[index++];
#endif
        item->push_back(val);
    }
    return item;
}

std::string UInt8ByteItem::print(std::string frontStr /*= ""*/) const
{
    // "< U8[2] 12 3 >"
    std::string formatStr = frontStr + "< U8";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] ");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += (std::to_string(m_data[i]) + " ");
    formatStr += ">";
    return formatStr;
}

size_t UInt8ByteItem::GetArraySize() const
{
    return m_data.size();
}

void UInt8ByteItem::clear()
{
    m_data.clear();
}

void UInt8ByteItem::push_back(uint64_t val)
{
    m_data.push_back(val);
}

void UInt8ByteItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void UInt8ByteItem::set(uint64_t val, size_t index /*= 0*/)
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

uint64_t UInt8ByteItem::get(size_t index /*= 0*/)
{
    if (GetArraySize() == 0)
        return 0;
    if (index >= GetArraySize())
        index = GetArraySize() - 1;
    return m_data[index];
}

void UInt8ByteItem::insert(size_t pos, uint64_t val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void UInt8ByteItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}
