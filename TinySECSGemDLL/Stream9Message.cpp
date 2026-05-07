#include "Stream9Message.h"
#include "BaseItem.h"
#include "BinaryItem.h"
#include "ListItem.h"


TinySECSGem::SECSMessage TinySECSGem::Stream9Message::GenerateS9F1Message(BlockHeader mheader)
{
    SECSMessage msg;
    BlockHeader header;
    header.Set_DataMessage(mheader.getSessionId(), false, 9, 1, mheader.getTransactionNum());
    msg.SetHead(header);
    auto body = std::make_shared<BinaryItem>((BYTE*)&mheader, (BYTE*)(&mheader)+sizeof(BlockHeader));
    msg.SetData(body);
    return msg;
}

TinySECSGem::SECSMessage TinySECSGem::Stream9Message::GenerateS9F3Message(BlockHeader mheader)
{
    SECSMessage msg;
    BlockHeader header;
    header.Set_DataMessage(mheader.getSessionId(), false, 9, 3, mheader.getTransactionNum());
    msg.SetHead(header);
    auto body = std::make_shared<BinaryItem>((BYTE*)&mheader, (BYTE*)(&mheader) + sizeof(BlockHeader));
    msg.SetData(body);
    return msg;
}

TinySECSGem::SECSMessage TinySECSGem::Stream9Message::GenerateS9F5Message(BlockHeader mheader)
{
    SECSMessage msg;
    BlockHeader header;
    header.Set_DataMessage(mheader.getSessionId(), false, 9, 5, mheader.getTransactionNum());
    msg.SetHead(header);
    auto body = std::make_shared<BinaryItem>((BYTE*)&mheader, (BYTE*)(&mheader) + sizeof(BlockHeader));
    msg.SetData(body);
    return msg;
}

TinySECSGem::SECSMessage TinySECSGem::Stream9Message::GenerateS9F7Message(BlockHeader mheader)
{
    SECSMessage msg;
    BlockHeader header;
    header.Set_DataMessage(mheader.getSessionId(), false, 9, 7, mheader.getTransactionNum());
    msg.SetHead(header);
    auto body = std::make_shared<BinaryItem>((BYTE*)&mheader, (BYTE*)(&mheader) + sizeof(BlockHeader));
    msg.SetData(body);
    return msg;
}

TinySECSGem::SECSMessage TinySECSGem::Stream9Message::GenerateS9F9Message(BlockHeader mheader)
{
    SECSMessage msg;
    BlockHeader header;
    header.Set_DataMessage(mheader.getSessionId(), false, 9, 9, mheader.getTransactionNum());
    msg.SetHead(header);
    auto body = std::make_shared<BinaryItem>((BYTE*)&mheader, (BYTE*)(&mheader) + sizeof(BlockHeader));
    msg.SetData(body);
    return msg;
}

TinySECSGem::SECSMessage TinySECSGem::Stream9Message::GenerateS9F11Message(BlockHeader mheader)
{
    SECSMessage msg;
    BlockHeader header;
    header.Set_DataMessage(mheader.getSessionId(), false, 9, 11, mheader.getTransactionNum());
    msg.SetHead(header);
    auto body = std::make_shared<BinaryItem>((BYTE*)&mheader, (BYTE*)(&mheader) + sizeof(BlockHeader));
    msg.SetData(body);
    return msg;
}

TinySECSGem::SECSMessage TinySECSGem::Stream9Message::GenerateS9F13Message(BlockHeader mheader, std::string mexp, std::string edid)
{
    SECSMessage msg;
    BlockHeader header;
    header.Set_DataMessage(mheader.getSessionId(), false, 9, 13, mheader.getTransactionNum());
    msg.SetHead(header);
    auto body = std::make_shared<ListItem>();
    body->push_back(new ASCIIItem(mexp));
    body->push_back(new ASCIIItem(edid));
    msg.SetData(body);
    return msg;
}
