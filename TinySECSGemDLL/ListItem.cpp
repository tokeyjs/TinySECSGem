#include "ListItem.h"

using namespace TinySECSGem;


ListItem::ListItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_LIST;
}

ListItem::ListItem(BaseItem* val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_LIST;
    push_back(val);
}

ListItem::ListItem(const ListItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_LIST;
    clear();
    for (int i = 0; i < item.GetArraySize(); i++)
    {
        push_back(item.get<BaseItem>(i)->clone());
    }
}

ListItem& ListItem::operator=(const ListItem& item)
{
    if (this == &item)
        return *this;
    clear();
    for (int i = 0; i < item.GetArraySize(); i++)
    {
        push_back(item.get<BaseItem>(i)->clone());
    }
    return *this;
}

ListItem& ListItem::operator=(BaseItem* val)
{
    if (val == this)
        return *this;
    clear();
    push_back(val);
    return *this;
}

ListItem::~ListItem()
{
    clear();
}


BaseItem* ListItem::clone()
{
    ListItem* item = new ListItem();
    item->clear();
    for (int i = 0; i < GetArraySize(); i++)
    {
        item->push_back(m_data[i]->clone());
    }
    return item;
}

size_t ListItem::totalByteSize() const
{
    size_t rtn = 1;
    if (GetArraySize() <= 0xFFU)
        rtn += 1;
    else if (GetArraySize() <= 0xFFFFU)
        rtn += 2;
    else if (GetArraySize() <= 0xFFFFFFU)
        rtn += 3;
    else
        rtn = 0;
    for (int i = 0; i < GetArraySize(); i++)
        rtn += m_data[i]->totalByteSize();
    return rtn;
}


int ListItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    int costSize = BaseItem::SerializationItemHeader(buff, buff + buffSize, m_eItemType, 1, GetArraySize());
    if (costSize < 0)
        return -1;
    index += costSize;
    for (int i = 0; i < GetArraySize(); i++)
    {
        const int childSize = m_data[i]->Serialization(buff + index, buffSize - index);
        if (childSize <= 0)
            return -1;
        index += static_cast<size_t>(childSize);
    }
    return totalByteSize();
}


ListItem* ListItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    size_t index = 0;
    EnumSECSItemType type;
    size_t lengthByteSize = 0;
    size_t lengthByte = 0;
    if (!BaseItem::DeserializationItemHeader(bufStart, bufEnd, type, lengthByteSize, lengthByte))
        return nullptr;
    if (type != EnumSECSItemType::ItemType_LIST)
        return nullptr;
    if ((bufEnd - bufStart) < (1 + lengthByteSize + lengthByte))
        return nullptr;
    index += (1 + lengthByteSize);
    size_t length = 0; // size
    length = lengthByte;
    ListItem* item = new ListItem();
    for (int i = 0; i < length; i++)
    {
        BaseItem* val = nullptr;
        EnumSECSItemType itemType;
        size_t itemLengthByteSize = 0;
        size_t itemLengthByte = 0;
        if (!BaseItem::DeserializationItemHeader(bufStart + index, bufEnd, itemType, itemLengthByteSize, itemLengthByte))
        {
            delete item;
            return nullptr;
        }
        switch (itemType)
        {
        case EnumSECSItemType::ItemType_ASCII:
        {
            val = ASCIIItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_JIS:
        {
            val = JIS8Item::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_UNICODE:
        {
            val = UnicodeItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_BINARY:
        {
            val = BinaryItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_BOOLEAN:
        {
            val = BooleanItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_INT1:
        {
            val = Int1ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_INT2:
        {
            val = Int2ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_INT4:
        {
            val = Int4ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_INT8:
        {
            val = Int8ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_UINT1:
        {
            val = UInt1ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_UINT2:
        {
            val = UInt2ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_UINT4:
        {
            val = UInt4ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_UINT8:
        {
            val = UInt8ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_FLOAT4:
        {
            val = Float4ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_FLOAT8:
        {
            val = Float8ByteItem::Deserialization(bufStart + index, bufEnd);
            break;
        }
        case EnumSECSItemType::ItemType_LIST:
        {
            val = Deserialization(bufStart + index, bufEnd);
            break;
        }
        default:
            break;
        }

        if (!val)
        {
            delete item;
            return nullptr;
        }
        const size_t itemSize = val->totalByteSize();
        if (itemSize == 0)
        {
            delete item;
            return nullptr;
        }
        index += itemSize;
        item->push_back(val);
    }
    return item;
}

std::string ListItem::print(std::string frontStr /*= ""*/) const
{
    // "< L[2] 12 3 >"
    std::string formatStr = frontStr + "< L";
    formatStr += ("[" + std::to_string(GetArraySize()) + "]");
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += ("\n" + m_data[i]->print("    " + frontStr));
    formatStr += ((GetArraySize() > 0 ? "\n" + frontStr : " ") + ">");
    return formatStr;
}


size_t ListItem::GetArraySize() const
{
    return m_data.size();
}


void ListItem::clear()
{
    for (int i = 0; i < GetArraySize(); i++)
    {
        delete m_data[i];
    }
    m_data.clear();
}

void ListItem::push_back(BaseItem* val)
{
    m_data.push_back(val);
}

void ListItem::pop_back()
{
    if (m_data.empty())
        return;
    delete m_data.back();
    m_data.pop_back();
}

void ListItem::set(BaseItem* val, size_t index /*= 0*/)
{
    if (index >= GetArraySize())
    {
        push_back(val);
    }
    else
    {
        delete m_data[index];
        m_data[index] = val;
    }
}

void ListItem::insert(size_t pos, BaseItem* val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void ListItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    delete m_data[pos];
    m_data.erase(m_data.begin() + pos);
}


