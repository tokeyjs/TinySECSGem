#include "TopItem.h"

using namespace TinySECSGem;



TopItem::TopItem()
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_TOP;
}

TopItem::TopItem(BaseItem* val)
    :BaseItem()
{
    m_eItemType = EnumSECSItemType::ItemType_TOP;
    push_back(val);
}

TopItem::TopItem(const TopItem& item)
{
    m_eItemType = EnumSECSItemType::ItemType_TOP;
    clear();
    for (int i = 0; i < item.GetArraySize(); i++)
    {
        push_back(item.get<BaseItem>(i)->clone());
    }
}

TopItem& TopItem::operator=(const TopItem& item)
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

TopItem& TopItem::operator=(TopItem* val)
{
    if (val == this)
        return *this;
    clear();
    for (int i = 0; i < val->GetArraySize(); i++)
    {
        push_back(val->get<BaseItem>(i)->clone());
    }
    return *this;
}

TopItem::~TopItem()
{
    clear();
}

BaseItem* TopItem::clone()
{
    TopItem* item = new TopItem();
    item->clear();
    for (int i = 0; i < GetArraySize(); i++)
    {
        item->push_back(m_data[i]->clone());
    }
    return item;
}

size_t TopItem::totalByteSize() const
{
    size_t rtn = 0;
    for (int i = 0; i < GetArraySize(); i++)
        rtn += m_data[i]->totalByteSize();
    return rtn;
}

int TopItem::Serialization(BYTE* buff, size_t buffSize)
{
    if (totalByteSize() > buffSize || totalByteSize() == 0)
        return -1; 
    size_t index = 0;
    for (int i = 0; i < GetArraySize(); i++)
    {
        const int childSize = m_data[i]->Serialization(buff + index, buffSize - index);
        if (childSize <= 0)
            return -1;
        index += static_cast<size_t>(childSize);
    }
    return totalByteSize();
}

TopItem* TopItem::Deserialization(BYTE* bufStart, BYTE* bufEnd)
{
    if (bufStart >= bufEnd)
        return nullptr;
    TopItem* item = new TopItem();
    size_t index = 0;
    while (bufStart + index < bufEnd)
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
            val = ListItem::Deserialization(bufStart + index, bufEnd);
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

std::string TopItem::print(std::string frontStr /*= ""*/) const
{
    // "< L[2] 12 3 >"
    std::string formatStr = "";
    for (int i = 0; i < GetArraySize(); i++)
        formatStr += (m_data[i]->print() + "\n");
    return formatStr;
}

size_t TopItem::GetArraySize() const
{
    return m_data.size();
}

void TopItem::clear()
{
    for (int i = 0; i < GetArraySize(); i++)
    {
        delete m_data[i];
    }
    m_data.clear();
}

void TopItem::push_back(BaseItem* val)
{
    m_data.push_back(val);
}

void TopItem::pop_back()
{
    if (m_data.empty())
        return;
    delete m_data.back();
    m_data.pop_back();
}

void TopItem::set(BaseItem* val, size_t index /*= 0*/)
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

void TopItem::insert(size_t pos, BaseItem* val)
{
    if (pos > m_data.size())
        return;
    m_data.insert(m_data.begin() + pos, val);
}

void TopItem::erase(size_t pos)
{
    if (pos >= m_data.size())
        return;
    delete m_data[pos];
    m_data.erase(m_data.begin() + pos);
}
