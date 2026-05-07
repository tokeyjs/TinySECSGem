#include "Int8ByteItem.h"

using namespace TinySECSGem;


Int8ByteItem::Int8ByteItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_INT8;
}

Int8ByteItem::Int8ByteItem(int64_t val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_INT8;
    push_back(val);
}


Int8ByteItem::Int8ByteItem(const Int8ByteItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_INT8;
    m_data = item.m_data;
}

Int8ByteItem& Int8ByteItem::operator=(const Int8ByteItem& item)
{
    m_data = item.m_data;
    return *this;
}

Int8ByteItem& Int8ByteItem::operator=(int64_t val)
{
    clear();
    push_back(val);
    return *this;
}

Int8ByteItem::~Int8ByteItem()
{
}

BaseItem* Int8ByteItem::clone()
{
    Int8ByteItem* item = new Int8ByteItem();
    item->m_data = m_data;
    return item;
}

int64_t& Int8ByteItem::operator[](size_t index) {
    return m_data[index];
}


size_t Int8ByteItem::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(int64_t);
    if (GetArraySize() * sizeof(int64_t) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(int64_t) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(int64_t) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}


int Int8ByteItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(int64_t), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        int64_t val = (*this)[i];
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

Int8ByteItem* Int8ByteItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_INT8)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(int64_t) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(int64_t);
    Int8ByteItem* item = new Int8ByteItem();
    for (int i = 0; i < length; i++)
    {
        int64_t val = 0;
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

std::string Int8ByteItem::print(std::string frontStr /*= ""*/) const
{
    // "< I8[2] 12 3 >"
    std::string formatStr = frontStr + "< I8";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] ");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += (std::to_string(m_data[i]) + " ");
    formatStr += ">";
    return formatStr;
}

size_t Int8ByteItem::GetArraySize() const
{
    return m_data.size();
}

void Int8ByteItem::clear()
{
    m_data.clear();
}

void Int8ByteItem::push_back(int64_t val)
{
    m_data.push_back(val);
}

void Int8ByteItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void Int8ByteItem::set(int64_t val, size_t index /*= 0*/)
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

int64_t Int8ByteItem::get(size_t index /*= 0*/)
{
    if (GetArraySize() == 0)
        return 0;
    if (index >= GetArraySize())
        index = GetArraySize() - 1;
    return m_data[index];
}

void Int8ByteItem::insert(size_t pos, int64_t val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void Int8ByteItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}
