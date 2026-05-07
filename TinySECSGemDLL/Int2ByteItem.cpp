#include "Int2ByteItem.h"

using namespace TinySECSGem;

Int2ByteItem::Int2ByteItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_INT2;
}

Int2ByteItem::Int2ByteItem(int16_t val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_INT2;
    push_back(val);
}


Int2ByteItem::Int2ByteItem(const Int2ByteItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_INT2;
    m_data = item.m_data;
}

Int2ByteItem& Int2ByteItem::operator=(const Int2ByteItem& item)
{
    m_data = item.m_data;
    return *this;
}

Int2ByteItem& Int2ByteItem::operator=(int16_t val)
{
    clear();
    push_back(val);
    return *this;
}

Int2ByteItem::~Int2ByteItem()
{
}

BaseItem* Int2ByteItem::clone()
{
    Int2ByteItem* item = new Int2ByteItem();
    item->m_data = m_data;
    return item;
}

int16_t& Int2ByteItem::operator[](size_t index) {
    return m_data[index];
}

size_t Int2ByteItem::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(int16_t);
    if (GetArraySize() * sizeof(int16_t) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(int16_t) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(int16_t) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}

int Int2ByteItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(int16_t), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        int16_t val = (*this)[i];
        unsigned char* valPtr = (unsigned char*)&val;
#if SECS_ENDIAN == SECS_LITTLE_ENDIAN
        buff[index++] = valPtr[1];
        buff[index++] = valPtr[0];
#else
        buff[index++] = valPtr[0];
        buff[index++] = valPtr[1];
#endif

    }
    return totalByteSize();
}

Int2ByteItem* Int2ByteItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_INT2)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(int16_t) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(int16_t);
    Int2ByteItem* item = new Int2ByteItem();
    for (int i = 0; i < length; i++)
    {
        int16_t val = 0;
        unsigned char* valPtr = (unsigned char*)&val;
#if SECS_ENDIAN == SECS_LITTLE_ENDIAN
        valPtr[1] = bufStart[index++];
        valPtr[0] = bufStart[index++];
#else
        valPtr[0] = bufStart[index++];
        valPtr[1] = bufStart[index++];
#endif
        item->push_back(val);
    }
    return item;
}

std::string Int2ByteItem::print(std::string frontStr /*= ""*/) const
{
    // "< I2[2] 12 3 >"
    std::string formatStr = frontStr + "< I2";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] ");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += (std::to_string(m_data[i]) + " ");
    formatStr += ">";
    return formatStr;
}

size_t Int2ByteItem::GetArraySize() const
{
    return m_data.size();
}

void Int2ByteItem::clear()
{
    m_data.clear();
}

void Int2ByteItem::push_back(int16_t val)
{
    m_data.push_back(val);
}

void Int2ByteItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void Int2ByteItem::set(int16_t val, size_t index /*= 0*/)
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

int16_t Int2ByteItem::get(size_t index /*= 0*/)
{
    if (GetArraySize() == 0)
        return 0;
    if (index >= GetArraySize())
        index = GetArraySize() - 1;
    return m_data[index];
}

void Int2ByteItem::insert(size_t pos, int16_t val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void Int2ByteItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}
