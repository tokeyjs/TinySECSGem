#include "Int1ByteItem.h"

using namespace TinySECSGem;


Int1ByteItem::Int1ByteItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_INT1;
}

Int1ByteItem::Int1ByteItem(int8_t val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_INT1;
    push_back(val);
}


Int1ByteItem::Int1ByteItem(const Int1ByteItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_INT1;
    m_data = item.m_data;
}

Int1ByteItem& Int1ByteItem::operator=(const Int1ByteItem& item)
{
    m_data = item.m_data;
    return *this;
}

Int1ByteItem& Int1ByteItem::operator=(int8_t val)
{
    clear();
    push_back(val);
    return *this;
}

Int1ByteItem::~Int1ByteItem()
{
}

BaseItem* Int1ByteItem::clone()
{
    Int1ByteItem* item = new Int1ByteItem();
    item->m_data = m_data;
    return item;
}

int8_t& Int1ByteItem::operator[](size_t index) {
    return m_data[index];
}


size_t Int1ByteItem::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(int8_t);
    if (GetArraySize() * sizeof(int8_t) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(int8_t) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(int8_t) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}

int Int1ByteItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(int8_t), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        buff[index++] = (*this)[i];
    }
    return totalByteSize();
}

Int1ByteItem* Int1ByteItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_INT1)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(int8_t) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(int8_t);
    Int1ByteItem* item = new Int1ByteItem();
    for (int i = 0; i < length; i++)
    {
        int8_t val = bufStart[index++];
        item->push_back(val);
    }
    return item;
}

std::string Int1ByteItem::print(std::string frontStr /*= ""*/) const
{
    // "< I1[2] 12 3 >"
    std::string formatStr = frontStr + "< I1";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] ");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += (std::to_string(m_data[i]) + " ");
    formatStr += ">";
    return formatStr;
}

size_t Int1ByteItem::GetArraySize() const
{
    return m_data.size();
}

void Int1ByteItem::clear()
{
    m_data.clear();
}

void Int1ByteItem::push_back(int8_t val)
{
    m_data.push_back(val);
}

void Int1ByteItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void Int1ByteItem::set(int8_t val, size_t index /*= 0*/)
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

int8_t Int1ByteItem::get(size_t index /*= 0*/)
{
    if (GetArraySize() == 0)
        return 0;
    if (index >= GetArraySize())
        index = GetArraySize() - 1;
    return m_data[index];
}

void Int1ByteItem::insert(size_t pos, int8_t val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void Int1ByteItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}
