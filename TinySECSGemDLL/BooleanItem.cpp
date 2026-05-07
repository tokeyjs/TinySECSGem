#include "BooleanItem.h"

using namespace TinySECSGem;

BooleanItem::BooleanItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_BOOLEAN;
}

BooleanItem::BooleanItem(bool val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_BOOLEAN;
    push_back(val);
}

BooleanItem::BooleanItem(const BooleanItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_BOOLEAN;
    m_data = item.m_data;
}

BooleanItem& BooleanItem::operator=(const BooleanItem& item)
{
    m_data = item.m_data;
    return *this;
}

BooleanItem& BooleanItem::operator=(bool val)
{
    clear();
    m_data.push_back(val);
    return *this;
}

BooleanItem::~BooleanItem()
{
}

BaseItem* BooleanItem::clone()
{
    BooleanItem* item = new BooleanItem();
    item->m_data = m_data;
    return item;
}

const bool BooleanItem::operator[](size_t index)
{
    return m_data[index];
}

size_t BooleanItem::totalByteSize() const
{
    size_t rtn = 1 + GetArraySize() * sizeof(bool);
    if (GetArraySize() * sizeof(bool) <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() * sizeof(bool) <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() * sizeof(bool) <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    return rtn;
}

int BooleanItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1;
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, sizeof(bool), GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        buff[index++] = (*this)[i];
    }
    return totalByteSize();
}

BooleanItem* BooleanItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_BOOLEAN)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte)
        || lengthByte % sizeof(bool) != 0)
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte / sizeof(bool);
    BooleanItem* item = new BooleanItem();
    item->clear();
    for (int i = 0; i < length; i++)
    {
        bool val = bufStart[index++];
        item->push_back(val);
    }
    return item;
}

std::string BooleanItem::print(std::string frontStr/* = ""*/) const
{
    // "< BOOLEAN[2] T F >"
    std::string formatStr = frontStr + "< BOOLEAN";
    formatStr += ("[" + std::to_string(GetArraySize()) + "] ");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += m_data[i] ? "T " : "F ";
    formatStr += ">";
    return formatStr;
}

size_t BooleanItem::GetArraySize() const
{
    return m_data.size();
}

void BooleanItem::clear()
{
    m_data.clear();
}

void BooleanItem::push_back(bool val)
{
    m_data.push_back(val);
}

void BooleanItem::pop_back()
{
    if (m_data.empty())
        return;
    m_data.pop_back();
}

void BooleanItem::insert(size_t pos, bool val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void BooleanItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    m_data.erase(m_data.begin() + pos);
}

bool BooleanItem::get(size_t index /*= 0*/)
{
    if (GetArraySize() == 0)
        return false;
    if (index >= GetArraySize())
        index = GetArraySize() - 1;
    return m_data[index];
}


void BooleanItem::set(bool val, size_t index /*= 0*/)
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

void BooleanItem::setAt(size_t index, bool val)
{
    if (index >= GetArraySize())
        return;
    m_data[index] = val;
}
