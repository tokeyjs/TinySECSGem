#include "SecsGemLibrary.h"
#include <Windows.h>

#include <iostream>
#include <iomanip>
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <cstdint>

using namespace TinySECSGem;

// ---------------------------------------------------------------------------
// DemoValidator -- validates incoming SECS messages
// ---------------------------------------------------------------------------
class DemoValidator : public ISECSMsgValidator
{
public:
    DemoValidator() {}
    virtual ~DemoValidator() {}

    virtual bool CheckStream(const SECSMessage& message)
    {
        int stream = message.GetStream();
        if (stream == 2 || stream == 3
            || stream == 4 || stream == 8
            || stream == 12 || stream == 14)
        {
            return false;
        }
        return true;
    }

    virtual bool CheckFunction(const SECSMessage& message)
    {
        int stream = message.GetStream();
        int function = message.GetFunction();
        std::map<int, std::set<int>> mapSF = std::map<int, std::set<int>>
        {
            {1, {0,1,2,3,4,5,6,11,12,13,14,15,16,17,18}},
            {5, {0,3,4,5,6,7,8}},
            {7, {0,1,2,3,4,5,6,19,20}},
        };
        if (mapSF.find(stream) != mapSF.end()
            && mapSF[stream].find(function) != mapSF[stream].end())
            return true;
        return false;
    }

    virtual bool CheckMessageData(const SECSMessage& message)
    {
        if (message.GetStream() == 1 && message.GetFunction() == 3)
        {
            auto data = message.GetData();
            if (data == nullptr)
                return false;
            TopItem* top = data->as<TopItem>();
            if (top->GetArraySize() <= 0 || top->GetArraySize() > 1)
                return false;
            ListItem* list = top->get<ListItem>(0);
            if (list == nullptr)
                return false;
            if (list->GetArraySize() == 0)
                return true;
            if (list->get<UInt4ByteItem>(0))
                return true;
            else
                return false;
        }
        return true;
    }
};

// ---------------------------------------------------------------------------
// DemoHandler -- handles SECS events and messages
// ---------------------------------------------------------------------------
class DemoHandler final : public ISecsGemHandler
{
public:
    DemoHandler()
        : m_gem(nullptr)
    {
    }

    DemoHandler(SecsGem* gem)
        : m_gem(gem)
    {
    }

    void SetSECSGem(SecsGem* gem)
    {
        m_gem = gem;
    }

    void OnConnected() override
    {
        std::cout << "[Demo] TCP connected." << std::endl;
    }

    void OnSelected() override
    {
        std::cout << "[Demo] HSMS selected." << std::endl;
    }

    void OnControlMessage(const BlockHeader& header) override
    {
    }

    void OnPrimaryMessage(const SECSMessage& msg) override
    {
        /// 
        /// Note: If the received message contains a body section, 
        /// the library will always parse it into a tree structure with TopItem as the outermost layer.
        /// 

        if (msg.GetStream() == 1 && msg.GetFunction() == 1)
        {
            // Reply to S1F1 with MDLN and SOFTREV
            auto replyBody = std::make_shared<ListItem>();
            replyBody->push_back(new ASCIIItem("CPP SECS/GEM DLL"));
            replyBody->push_back(new ASCIIItem("TEST APP"));

            SECSMessage reply;
            BlockHeader replyHeader;
            replyHeader.Set_DataMessage_GenSecondaryMsgHeader(msg.GetHead());
            reply.SetHead(replyHeader);
            reply.SetData(replyBody);

            if (m_gem)
                m_gem->SendSECSMessage(reply);
        }
        else if (msg.GetStream() == 1 && msg.GetFunction() == 3)
        {
            std::vector<int> val;
            try
            {
                ListItem* list = msg.GetData()->as<TopItem>()->get<ListItem>(0);
                if (list->GetArraySize() == 0)
                {
                    auto replyBody = std::make_shared<ListItem>();
                    for (int i = 0; i < 12; i++)
                        replyBody->push_back(new UInt4ByteItem(i * 10));
                    SECSMessage reply;
                    BlockHeader replyHeader;
                    replyHeader.Set_DataMessage_GenSecondaryMsgHeader(msg.GetHead());
                    reply.SetHead(replyHeader);
                    reply.SetData(replyBody);
                    if (m_gem)
                        m_gem->SendSECSMessage(reply);
                }
                else
                {
                    for (size_t i = 0; i < list->GetArraySize(); i++)
                    {
                        UInt4ByteItem* item = list->get<UInt4ByteItem>(i);
                        if (item)
                            val.push_back(item->get());
                    }
                    auto replyBody = std::make_shared<ListItem>();
                    for (size_t i = 0; i < val.size(); i++)
                        replyBody->push_back(new UInt4ByteItem(val[i]));
                    SECSMessage reply;
                    BlockHeader replyHeader;
                    replyHeader.Set_DataMessage_GenSecondaryMsgHeader(msg.GetHead());
                    reply.SetHead(replyHeader);
                    reply.SetData(replyBody);
                    if (m_gem)
                        m_gem->SendSECSMessage(reply);
                }
            }
            catch (...)
            {
            }
        }
    }

    void OnSecondaryMessage(const SECSMessage& msg) override
    {
    }

    void OnProtocolError(const char* text) override
    {
        std::cout << "[Demo] Protocol error: " << text << std::endl;
    }

    void OnTimeout(const char* timerName, uint32_t tid) override
    {
        std::cout << "[Demo] Timeout " << timerName << ", transaction=0x"
            << std::hex << tid << std::dec << std::endl;
    }

    void OnDisconnected() override
    {
        std::cout << "[Demo] Disconnected." << std::endl;
    }

private:
    SecsGem* m_gem;
};

// ---------------------------------------------------------------------------
// Test1: Passive mode (server) -- blocks indefinitely
// ---------------------------------------------------------------------------
void Test1()
{
    std::cout << "\n===== Test1: Passive Mode (Server) =====\n" << std::endl;

    DemoHandler handler;
    DemoValidator validator;

    SecsGem gem;

    SocketParameter parameter;
    parameter.m_mode = EnumHSMSMode::EnumHSMSMode_Passive;
    parameter.m_port = 5000;
    parameter.m_deviceId = 0;
    parameter.m_autoSelect = true;
    parameter.m_autoReconnect = true;
    strcpy_s(parameter.m_ip, "0.0.0.0");

    gem.SetParameter(parameter);

    handler.SetSECSGem(&gem);
    gem.SetHandler(&handler);

    gem.SetSECSMsgValidator(&validator);

    if (!gem.Start())
    {
        std::cout << "[Demo] Start failed." << std::endl;
        return;
    }
    std::cout << "[Demo] Server started on port 5000" << std::endl;
    std::cout << "[Demo] Running: " << (gem.IsRunning() ? "yes" : "no") << std::endl;

    while (true)
        ::Sleep(1000);
}

// ---------------------------------------------------------------------------
// Test2: Active mode (client) -- blocks indefinitely
// ---------------------------------------------------------------------------
void Test2()
{
    std::cout << "\n===== Test2: Active Mode (Client) =====\n" << std::endl;

    DemoHandler handler;
    DemoValidator validator;

    SecsGem gem;

    SocketParameter parameter;
    parameter.m_mode = EnumHSMSMode::EnumHSMSMode_Active;
    parameter.m_port = 5000;
    parameter.m_deviceId = 0;
    parameter.m_autoSelect = true;
    parameter.m_autoReconnect = true;
    strcpy_s(parameter.m_ip, "127.0.0.1");

    gem.SetParameter(parameter);

    handler.SetSECSGem(&gem);
    gem.SetHandler(&handler);

    gem.SetSECSMsgValidator(&validator);

    if (!gem.Start())
    {
        std::cout << "[Demo] Start failed." << std::endl;
        return;
    }
    std::cout << "[Demo] Client connecting to 127.0.0.1:5000" << std::endl;

    while (true)
        ::Sleep(1000);
}

// ---------------------------------------------------------------------------
// Test3: Item tree construction and parsing
// ---------------------------------------------------------------------------
void Test3()
{
    std::cout << "\n===== Test3: Item Tree Construction & Parsing =====\n" << std::endl;

    {
        std::cout << "eg 1:" << std::endl;
        // eg 1:
        // Build this tree:
        //   <L[4]
        //       <A[11] 'TinySECSGem'>
        //       <U4[0]>
        //       <U1[1] 34>
        //       <I4[2] 23 45>
        //   >
        ListItem* list = new ListItem();
        list->push_back(new ASCIIItem("TinySECSGem"));
        list->push_back(new UInt4ByteItem());
        list->push_back(new UInt1ByteItem(34));

        Int4ByteItem* i4 = new Int4ByteItem();
        i4->push_back(23);
        i4->push_back(45);
        list->push_back(i4);

        // Print the tree
        std::cout << "Constructed tree:" << std::endl;
        std::cout << list->print() << std::endl;

        // Parse elements safely
        std::cout << "Parsing elements:" << std::endl;
        if (list->GetArraySize() == 4)
        {
            // A[11]
            if (auto* item = list->get<ASCIIItem>(0))
                std::cout << "  [0] ASCIIItem: " << item->get() << std::endl;

            // U4[0]
            if (auto* item = list->get<UInt4ByteItem>(1))
                std::cout << "  [1] UInt4ByteItem: count=" << item->GetArraySize() << std::endl;

            // U1[1]
            if (auto* item = list->get<UInt1ByteItem>(2))
                std::cout << "  [2] UInt1ByteItem: " << (int)item->get() << std::endl;

            // I4[2]
            if (auto* item = list->get<Int4ByteItem>(3))
            {
                std::cout << "  [3] Int4ByteItem: ";
                for (size_t i = 0; i < item->GetArraySize(); i++)
                    std::cout << item->get(i) << " ";
                std::cout << std::endl;
            }
        }

        delete list;
    }

    {
        std::cout << "eg 2:" << std::endl;
        // eg 2:
        // Build this tree:
        //   <L[4]
        //       <A[11] 'TinySECSGem'>
        //       <U4[0]>
        //       <U1[1] 34>
        //       <I4[2] 23 45>
        //   >
        //   <L[3]
        //       <A[4] 'Test'>
        //       <BOOLEAN[1] T>
        //       <L[1]
        //           <U4[1] 12>
        //       >
        //   >
        //  tip: This structure is special. It does not look like a standard tree structure, 
        //  but it is valid under the E5 specification. 
        //  If an E5 message encounters this scenario, 
        //  it needs to be wrapped with a TopItem at the outermost layer.

        ListItem* list1 = new ListItem();
        list1->push_back(new ASCIIItem("TinySECSGem"));
        list1->push_back(new UInt4ByteItem());
        list1->push_back(new UInt1ByteItem(34));
        Int4ByteItem* i4 = new Int4ByteItem();
        i4->push_back(23);
        i4->push_back(45);
        list1->push_back(i4);

        ListItem* list2 = new ListItem();
        list2->push_back(new ASCIIItem("Test"));
        list2->push_back(new BooleanItem(true));
        ListItem* list2_1 = new ListItem();
        list2_1->push_back(new UInt4ByteItem(12));
        list2->push_back(list2_1);

        TopItem* top = new TopItem();
        top->push_back(list1);
        top->push_back(list2);

        // Print the tree
        std::cout << "Constructed tree:" << std::endl;
        std::cout << top->print() << std::endl;

        // Parse elements safely
        std::cout << "Parsing elements:" << std::endl;
        if (top->GetArraySize() == 2)
        {
            ListItem* parserList1 = top->get<ListItem>(0);
            ListItem* parserList2 = top->get<ListItem>(1);
            if (parserList1)
            {
                // A[11]
                if (auto* item = parserList1->get<ASCIIItem>(0))
                    std::cout << "  [0][0] ASCIIItem: " << item->get() << std::endl;

                // U4[0]
                if (auto* item = parserList1->get<UInt4ByteItem>(1))
                    std::cout << "  [0][1] UInt4ByteItem: count=" << item->GetArraySize() << std::endl;

                // U1[1]
                if (auto* item = parserList1->get<UInt1ByteItem>(2))
                    std::cout << "  [0][2] UInt1ByteItem: " << (int)item->get() << std::endl;

                // I4[2]
                if (auto* item = parserList1->get<Int4ByteItem>(3))
                {
                    std::cout << "  [0][3] Int4ByteItem: ";
                    for (size_t i = 0; i < item->GetArraySize(); i++)
                        std::cout << item->get(i) << " ";
                    std::cout << std::endl;
                }
            }

            if (parserList2)
            {
                // A[4]
                if (auto* item = parserList2->get<ASCIIItem>(0))
                    std::cout << "  [1][0] ASCIIItem: " << item->get() << std::endl;

                // BOOLEAN[1]
                if (auto* item = parserList2->get<BooleanItem>(1))
                    std::cout << "  [1][1] BooleanItem: " << (item->get() ? "true":"false") << std::endl;

                // U4[1]
                if (auto* item = parserList2->get<ListItem>(2)->get<UInt4ByteItem>(0))
                    std::cout << "  [1][2][0] UInt4ByteItem: " << (int)item->get() << std::endl;
            }

            delete top;
        }

    }
}

// ---------------------------------------------------------------------------
// Test4: Message serialization and deserialization
// ---------------------------------------------------------------------------
void Test4()
{
    std::cout << "\n===== Test4: Message Serialization & Deserialization =====\n" << std::endl;

    // Build a data message: S1F1 with ASCII body "Hello SECS"
    auto body = std::make_shared<ASCIIItem>("Hello SECS");

    SECSMessage msg;
    BlockHeader header;
    header.Set_DataMessage(0, false, 1, 1, 100);
    msg.SetHead(header);
    msg.SetData(body);

    std::cout << "Original: S" << msg.GetStream() << "F" << msg.GetFunction()
        << "  data: " << msg.GetData()->print() << std::endl;

    // Serialize to wire format
    auto wire = msg.SerializeForWire();
    if (wire.empty())
    {
        std::cout << "Serialization failed!" << std::endl;
        return;
    }

    std::cout << "Wire bytes (" << wire.size() << " bytes): ";
    for (size_t i = 0; i < wire.size(); i++)
        printf("%02X ", wire[i]);
    std::cout << std::endl;

    // Deserialize back
    bool ok = false;
    auto parsed = SecsMessageParser::ParseWireMessage(wire.data(), wire.size(), ok);
    if (!ok || !parsed)
    {
        std::cout << "Deserialization failed!" << std::endl;
        return;
    }

    std::cout << "Parsed back: S" << parsed->GetStream() << "F" << parsed->GetFunction()
        << "  data: " << (parsed->GetData() ? parsed->GetData()->print() : "(null)")
        << std::endl;
}

// ---------------------------------------------------------------------------
// Test5: All Item type demonstrations
// ---------------------------------------------------------------------------
void Test5()
{
    std::cout << "\n===== Test5: All SECS-II Item Types =====\n" << std::endl;

    // ASCIIItem
    {
        std::cout << "-- ASCIIItem --" << std::endl;
        ASCIIItem a("Hello");
        a.push_back('!');
        std::cout << "  string: " << a.get() << std::endl;
        ASCIIItem b = a + ASCIIItem(" World");
        std::cout << "  concat: " << b.get() << std::endl;
    }

    // BinaryItem
    {
        std::cout << "-- BinaryItem --" << std::endl;
        BYTE data[] = { 0xDE, 0xAD, 0xBE, 0xEF };
        BinaryItem bin(data, data + 4);
        std::cout << "  bytes: " << bin.print() << std::endl;
    }

    // BooleanItem
    {
        std::cout << "-- BooleanItem --" << std::endl;
        BooleanItem b;
        b.push_back(true);
        b.push_back(false);
        b.push_back(true);
        std::cout << "  values: " << b.print() << std::endl;
    }

    // Integer types
    {
        std::cout << "-- Integer types --" << std::endl;
        Int1ByteItem i1;
        i1.push_back(-1);
        i1.push_back(127);
        std::cout << "  I1: " << i1.print() << std::endl;

        Int2ByteItem i2;
        i2.push_back(-1000);
        i2.push_back(20000);
        std::cout << "  I2: " << i2.print() << std::endl;

        Int4ByteItem i4;
        i4.push_back(-100000);
        i4.push_back(500000);
        std::cout << "  I4: " << i4.print() << std::endl;

        Int8ByteItem i8;
        i8.push_back(9223372036854775807LL);
        std::cout << "  I8: " << i8.print() << std::endl;
    }

    // Unsigned integer types
    {
        std::cout << "-- Unsigned types --" << std::endl;
        UInt1ByteItem u1;
        u1.push_back(200);
        std::cout << "  U1: " << u1.print() << std::endl;

        UInt2ByteItem u2;
        u2.push_back(60000);
        std::cout << "  U2: " << u2.print() << std::endl;

        UInt4ByteItem u4;
        u4.push_back(3000000000U);
        std::cout << "  U4: " << u4.print() << std::endl;

        UInt8ByteItem u8;
        u8.push_back(10000000000000000000ULL);
        std::cout << "  U8: " << u8.print() << std::endl;
    }

    // Float types
    {
        std::cout << "-- Float types --" << std::endl;
        Float4ByteItem f4;
        f4.push_back(3.14159f);
        f4.push_back(2.71828f);
        std::cout << "  F4: " << f4.print() << std::endl;

        Float8ByteItem f8;
        f8.push_back(3.14159265358979);
        std::cout << "  F8: " << f8.print() << std::endl;
    }

    // Nested ListItem
    {
        std::cout << "-- Nested ListItem --" << std::endl;
        ListItem* outer = new ListItem();
        ListItem* inner1 = new ListItem();
        inner1->push_back(new ASCIIItem("nested"));
        inner1->push_back(new UInt4ByteItem(42));
        outer->push_back(inner1);

        ListItem* inner2 = new ListItem();
        inner2->push_back(new BinaryItem((BYTE*)"\x01\x02\x03", (BYTE*)"\x01\x02\x03" + 3));
        outer->push_back(inner2);

        std::cout << outer->print() << std::endl;
        delete outer;
    }
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "  TinySECSGemDLL v" << SecsGem::GetVersion() << " Demo" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Offline tests (no network required)
    Test3();
    Test4();
    Test5();

    // Online tests (blocking, requires network -- uncomment to run)
    //Test1();   // Passive mode (server)
    //Test2();   // Active mode (client)

    std::cout << "\nDemo complete." << std::endl;
    return 0;
}
