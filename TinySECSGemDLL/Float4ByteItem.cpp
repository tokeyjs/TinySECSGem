#include "Float4ByteItem.h"

using namespace TinySECSGem;


Float4ByteItem::Float4ByteItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_FLOAT4;
}

Float4ByteItem::Float4ByteItem(float val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_FLOAT4;
    push_back(val);
}


Float4ByteItem::Float4ByteItem(const Float4ByteItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_FLOAT4;
    m_data = item.m_data;
}

Float4ByteItem& Float4ByteItem::operator=(const Float4ByteItem& item)
{
    m_data = item.m_data;
    return *this;
}

Float4ByteItem& Float4ByteItem::operator=(float val)
{
    clear();
    push_back(val);
    return *this;
}

Float4ByteItem::~Float4ByteItem()
{
}

BaseItem* Float4ByteItem::clone()
{
    Float4ByteItem* item = new Float4ByteItem();
    item->m_data = m_data;
    return item;
}

float& Float4ByteItem::operator[](size_t index) {
    return m_data[index];
}

size_t Float4ByteItem::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(float);
    if (GetArraySize() * sizeof(float) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(float) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(float) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}

int Float4ByteItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(float), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        float val = (*this)[i];
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

Float4ByteItem* Float4ByteItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_FLOAT4)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(float) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(float);
    Float4ByteItem* item = new Float4ByteItem();
    for (int i = 0; i < length; i++)
    {
        float val = 0;
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

std::string Float4ByteItem::print(std::string frontStr /*= ""*/) const
{
    // "< F4[2] 12.1 3.2 >"
    std::string formatStr = frontStr + "< F4";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] ");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += (std::to_string(m_data[i]) + " ");
    formatStr += ">";
    return formatStr;
}

size_t Float4ByteItem::GetArraySize() const
{
    return m_data.size();
}

void Float4ByteItem::clear()
{
    m_data.clear();
}

void Float4ByteItem::push_back(float val)
{
    m_data.push_back(val);
}

void Float4ByteItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void Float4ByteItem::set(float val, size_t index /*= 0*/)
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

float Float4ByteItem::get(size_t index/* = 0*/)
{
    if (GetArraySize() == 0)
        return 0;
    if (index >= GetArraySize())
        index = GetArraySize() - 1;
    return m_data[index];
}

void Float4ByteItem::insert(size_t pos, float val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void Float4ByteItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}
