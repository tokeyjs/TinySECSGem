#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef SECS_BIG_ENDIAN
#define SECS_BIG_ENDIAN 1234
#define SECS_LITTLE_ENDIAN 4321
#endif

#if defined(_M_IX86) || defined(__386__) || defined(i386) || \
    defined(_M_X64) || defined(__x86_64__) || defined(__amd64__) || \
    defined(_M_ARM) || defined(__arm__) || defined(__aarch64__) || \
    defined(__ARMEL__) || defined(__AARCH64EL__)
#define SECS_ENDIAN SECS_LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__) || defined(_M_PPC) || defined(__ppc__) || \
      defined(__ARMEB__) || defined(__AARCH64EB__)
#define SECS_ENDIAN SECS_BIG_ENDIAN
#else
#error "Unknown endianness for this platform"
#endif

#ifdef SECSGEMDLL_EXPORTS
#define TinySECSGEM_API __declspec(dllexport)
#else
#define TinySECSGEM_API __declspec(dllimport)
#endif

#define TINYSECSGEM_VERSION_MAJOR 1
#define TINYSECSGEM_VERSION_MINOR 0
#define TINYSECSGEM_VERSION_PATCH 0

#define TINYSECSGEM_VERSION_STRING "1.0.0"

#define HSMSRECV_BUFFER_SIZE (8 * 1024 * 1024)
#define HSMSRECV_BUFFER_MAXSIZE (12 * 1024 * 1024)
#define SF_CODE(stream, function) (((stream) << 8) | (function))

namespace TinySECSGem
{
    enum class EnumSECSItemType
    {
        ItemType_LIST = 000,
        ItemType_BINARY = 010,
        ItemType_BOOLEAN = 011,
        ItemType_ASCII = 020,
        ItemType_JIS = 021,
        ItemType_UNICODE = 022,
        ItemType_INT8 = 030,
        ItemType_INT1 = 031,
        ItemType_INT2 = 032,
        ItemType_INT4 = 034,
        ItemType_FLOAT8 = 040,
        ItemType_FLOAT4 = 044,
        ItemType_UINT8 = 050,
        ItemType_UINT1 = 051,
        ItemType_UINT2 = 052,
        ItemType_UINT4 = 054,
        ItemType_TOP = 070,
        ItemType_UNKNOW = 071,
    };

    enum class EnumHSMSMode
    {
        EnumHSMSMode_Active = 0,
        EnumHSMSMode_Passive = 1,
    };

    enum class EnumHSMS_SType : uint8_t
    {
        STYPE_DATA_MSG = 0,
        STYPE_SELECT_REQ = 1,
        STYPE_SELECT_RSP = 2,
        STYPE_DESELECT_REQ = 3,
        STYPE_DESELECT_RSP = 4,
        STYPE_LINKTEST_REQ = 5,
        STYPE_LINKTEST_RSP = 6,
        STYPE_REJECT_REQ = 7,
        STYPE_SEPARATE_REQ = 9,
    };

    enum class EnumHSMS_SelectStatus : uint8_t
    {
        HSMS_SELECT_OK = 0,
        HSMS_SELECT_ACTIVE = 1,
        HSMS_SELECT_NOT_READY = 2,
        HSMS_SELECT_CONNECTED = 3,
    };

    enum class EnumHSMS_DeselectStatus : uint8_t
    {
        HSMS_DESELECT_OK = 0,
        HSMS_DESELECT_NOTCONNECT = 1,
        HSMS_DESELECT_ACTIVE = 2,
    };

    enum class EnumHSMS_RejectStatus : uint8_t
    {
        HSMS_NOT_SUPPORT_STYPE = 1,
        HSMS_NOT_SUPPORT_PTYPE = 2,
        HSMS_NOT_OPEN_TRANSACTION = 3,
        HSMS_NOT_SELECTED_ENTITY = 4,
    };

    enum class HsmsConnectionState
    {
        NotConnected,
        ConnectedNotSelected,
        Selected,
    };

    struct SocketParameter
    {
        EnumHSMSMode m_mode;
        int m_deviceId;
        int m_port;
        char m_ip[64];
        int m_t3timeout;
        int m_t5timeout;
        int m_t6timeout;
        int m_t7timeout;
        int m_t8timeout;
        int m_t1timeout;
        int m_maxConn;
        bool m_autoSelect;
        bool m_autoReconnect;

        SocketParameter()
            : m_mode(EnumHSMSMode::EnumHSMSMode_Passive),
            m_deviceId(0),
            m_port(5000),
            m_t3timeout(45000),
            m_t5timeout(10000),
            m_t6timeout(5000),
            m_t7timeout(10000),
            m_t8timeout(15000),
            m_t1timeout(1000),
            m_maxConn(1),
            m_autoSelect(true),
            m_autoReconnect(true)
        {
            std::memcpy(m_ip, "127.0.0.1", sizeof("127.0.0.1"));
        }
    };
}
