#include "BaseItem.h"
#include "BinaryItem.h"
#include "BooleanItem.h"
#include "ASCIIItem.h"
#include "UnicodeItem.h"
#include "JIS8Item.h"
#include "Int1ByteItem.h"
#include "Int2ByteItem.h"
#include "Int4ByteItem.h"
#include "Int8ByteItem.h"
#include "UInt1ByteItem.h"
#include "UInt2ByteItem.h"
#include "UInt4ByteItem.h"
#include "UInt8ByteItem.h"
#include "Float4ByteItem.h"
#include "Float8ByteItem.h"
#include "ListItem.h"
#include "TopItem.h"



TinySECSGem::BaseItem::BaseItem()
	: m_eItemType(EnumSECSItemType::ItemType_UNKNOW)
{
}

TinySECSGem::BaseItem::~BaseItem() 
{ 
}


bool TinySECSGem::BaseItem::DeserializationItemHeader(BYTE* bufStart, BYTE* bufEnd, EnumSECSItemType& format, size_t& lengthByteSize, size_t& lengthByte)
{
	size_t index = 0;
	if (bufEnd <= bufStart || !bufStart || !bufEnd)
		return false;
	
	format = (EnumSECSItemType)(bufStart[index] >> 2); 
	lengthByteSize = (bufStart[index] & 0x3); 
	index++;
	if ((bufEnd - bufStart) < (1 + lengthByteSize)
		|| lengthByteSize == 0)
		return false;
	lengthByte = 0;
	lengthByte = bufStart[index++];
	if (lengthByteSize >= 2)
	{
		lengthByte <<= 8;
		lengthByte |= bufStart[index++];
	}
	if (lengthByteSize == 3)
	{
		lengthByte <<= 8;
		lengthByte |= bufStart[index++];
	}
	return true;
}

int TinySECSGem::BaseItem::SerializationItemHeader(BYTE* bufStart, BYTE* bufEnd, EnumSECSItemType format, size_t singleTypeSize, size_t arraySize)
{
	if (!bufStart || !bufEnd || bufEnd - bufStart < 2)
		return -1; 
	size_t index = 0;
	BYTE lengthByteSize = 0;
	unsigned int length = arraySize * singleTypeSize;
	bufStart[index] = (BYTE)format;
	bufStart[index] <<= 2;

	if (length <= 0xFFU)
	{
		lengthByteSize = 1;
		bufStart[index] |= lengthByteSize;
		index++;
		if (bufEnd - bufStart < 1 + lengthByteSize)
			return -1;
		bufStart[index++] = (length & 0xFFU);
	}
	else if (length <= 0xFFFFU)
	{
		lengthByteSize = 2;
		bufStart[index] |= lengthByteSize;
		index++;
		if (bufEnd - bufStart < 1 + lengthByteSize)
			return -1;
		bufStart[index++] = ((length >> 8) & 0xFFU);
		bufStart[index++] = (length & 0xFFU);
	}
	else if (length <= 0xFFFFFFU)
	{
		lengthByteSize = 3;
		bufStart[index] |= lengthByteSize;
		index++;
		if (bufEnd - bufStart < 1 + lengthByteSize)
			return -1;
		bufStart[index++] = ((length >> 16) & 0xFFU);
		bufStart[index++] = ((length >> 8) & 0xFFU);
		bufStart[index++] = (length & 0xFFU);
	}
	else
	{
		return -1;
	}

	return 1 + lengthByteSize;
}


TinySECSGem::BaseItem* TinySECSGem::BaseItem::Deserialization(BYTE* startBufPos, BYTE* endBufPos, bool& bOk)
{
	if (startBufPos > endBufPos)
	{
		bOk = false;
		return nullptr;
	}

	if (startBufPos == endBufPos)
	{
		bOk = true;
		return nullptr;
	}

	bOk = true;
	BaseItem* item = TopItem::Deserialization(startBufPos, endBufPos);
	if (!item)
	{
		bOk = false;
		delete item;
		return nullptr;
	}
	return item;
}



TinySECSGem::EnumSECSItemType  TinySECSGem::BaseItem::getFormat() const
{ 
	return m_eItemType; 
}

