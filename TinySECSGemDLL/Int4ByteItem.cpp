#include "Int4ByteItem.h"

using namespace TinySECSGem;

Int4ByteItem::Int4ByteItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_INT4;
}

Int4ByteItem::Int4ByteItem(int32_t val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_INT4;
    push_back(val);
}


Int4ByteItem::Int4ByteItem(const Int4ByteItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_INT4;
    m_data = item.m_data;
}

Int4ByteItem& Int4ByteItem::operator=(const Int4ByteItem& item)
{
    m_data = item.m_data;
    return *this;
}

Int4ByteItem& Int4ByteItem::operator=(int32_t val)
{
    clear();
    push_back(val);
    return *this;
}

Int4ByteItem::~Int4ByteItem()
{
}

BaseItem* Int4ByteItem::clone()
{
    Int4ByteItem* item = new Int4ByteItem();
    item->m_data = m_data;
    return item;
}

int32_t& Int4ByteItem::operator[](size_t index) {
    return m_data[index];
}

size_t Int4ByteItem::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(int32_t);
    if (GetArraySize() * sizeof(int32_t) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(int32_t) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(int32_t) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}

int Int4ByteItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(int32_t), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        int32_t val = (*this)[i];
        unsigned char* valPtr = (unsigned char*)&val;
#if SECS_ENDIAN == SECS_LITTLE_ENDIAN
        buff[index++] = valPtr[3];
        buff[index++] = valPtr[2];
        buff[index++] = valPtr[1];
        buff[index++] = valPtr[0];
#else
        buff[index++] = valPtr[0];
        buff[index++] = valPtr[1];
        buff[index++] = valPtr[2];
        buff[index++] = valPtr[3];
#endif
    }
    return totalByteSize();
}

Int4ByteItem* Int4ByteItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_INT4)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(int32_t) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(int32_t);
    Int4ByteItem* item = new Int4ByteItem();
    for (int i = 0; i < length; i++)
    {
        int32_t val = 0;
        unsigned char* valPtr = (unsigned char*)&val;
#if SECS_ENDIAN == SECS_LITTLE_ENDIAN
        valPtr[3] = bufStart[index++];
        valPtr[2] = bufStart[index++];
        valPtr[1] = bufStart[index++];
        valPtr[0] = bufStart[index++];
#else
        valPtr[0] = bufStart[index++];
        valPtr[1] = bufStart[index++];
        valPtr[2] = bufStart[index++];
        valPtr[3] = bufStart[index++];
#endif
        item->push_back(val);
    }
    return item;
}

std::string Int4ByteItem::print(std::string frontStr /*= ""*/) const
{
    // "< I4[2] 12 3 >"
    std::string formatStr = frontStr + "< I4";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] ");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += (std::to_string(m_data[i]) + " ");
    formatStr += ">";
    return formatStr;
}

size_t Int4ByteItem::GetArraySize() const
{
    return m_data.size();
}

void Int4ByteItem::clear()
{
    m_data.clear();
}

void Int4ByteItem::push_back(int32_t val)
{
    m_data.push_back(val);
}

void Int4ByteItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void Int4ByteItem::set(int32_t val, size_t index /*= 0*/)
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

int32_t Int4ByteItem::get(size_t index /*= 0*/)
{
    if (GetArraySize() == 0)
        return 0;
    if (index >= GetArraySize())
        index = GetArraySize() - 1;
    return m_data[index];
}

void Int4ByteItem::insert(size_t pos, int32_t val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void Int4ByteItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}
