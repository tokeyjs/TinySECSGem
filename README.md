[Simplified Chinese](README_CN.md)

# TinySECSGem

![platform](https://img.shields.io/badge/platform-Windows-blue)
![compiler](https://img.shields.io/badge/VisualStudio-2015%2B-green)
![standard](https://img.shields.io/badge/SEMI-E5%20%7C%20E37-orange)
![license](https://img.shields.io/badge/license-MIT-brightgreen)

TinySECSGem is a lightweight Windows C++ library for SECS/HSMS communication between semiconductor equipment and host applications.

The current release focuses on the communication layer:

- **SEMI E37 / HSMS-SS** transport and session behavior
- **SEMI E5 / SECS-II** message serialization, parsing, and item trees

GEM behavior features from **SEMI E30** are planned but not implemented in this version. TinySECSGem is intended to help developers build SECS message exchange, integration prototypes, host-side tools, and equipment-side communication services without claiming complete GEM equipment behavior support.

------

## Project Status and Use Cases

TinySECSGem is suitable for:

- HSMS-SS Active or Passive TCP communication
- SECS-II data message construction and parsing
- Equipment-side or host-side SECS integration prototypes
- Windows DLL integration in Visual Studio projects
- Custom application-level validation with Stream 9 error replies

It is not yet a complete GEM implementation. Features such as GEM state/event models, collection events, alarms, reports, SML text parsing, and full production equipment behavior are roadmap items.

------

## Features

### Transport

- HSMS-SS Active and Passive modes
- TCP transport based on HP-Socket pull-mode APIs
- Configurable IP, port, device ID, and reconnect behavior; the current maximum connection count is `1` for HSMS-SS

### Session Behavior

- HSMS control messages:
  - Select.req / Select.rsp
  - Deselect.req / Deselect.rsp
  - Linktest.req / Linktest.rsp
  - Separate.req
  - Reject.req
- HSMS connection state management:
  - NotConnected
  - ConnectedNotSelected
  - Selected
- Timers for T3, T5, T6, T7, T8, and T1
- Automatic Primary / Secondary transaction matching
- Optional auto-select and auto-reconnect behavior

### Message Model

- SECS-II wire serialization and parsing
- Hierarchical Item Tree model for message body data
- `TopItem` is a virtual root node used only to expose parsed message data as one tree; it is not a real SECS-II item format sent on the wire
- Supported item types:
  - ListItem / TopItem
  - ASCIIItem
  - BinaryItem
  - BooleanItem
  - Int1ByteItem / Int2ByteItem / Int4ByteItem / Int8ByteItem
  - UInt1ByteItem / UInt2ByteItem / UInt4ByteItem / UInt8ByteItem
  - Float4ByteItem / Float8ByteItem

### Validation and Errors

- Custom `ISECSMsgValidator` hook
- Automatic Stream 9 error replies:
  - S9F3 for unsupported stream
  - S9F5 for unsupported function
  - S9F7 for invalid data
- Protocol error and timeout callbacks through `ISecsGemHandler`

------

## Repository Layout

```text
TinySECSGemDLL/               # Library source code and Visual Studio project
Include/TinySECSGemDLL/       # Public headers copied by the post-build step
Test/                         # Console demo and integration sample
third_party/Include/HPSocket/ # HP-Socket headers
third_party/Lib/HPSocket/     # Prebuilt HP-Socket static libraries
Bin/                          # Runtime output copied by the build
Lib/                          # TinySECSGemDLL import libraries and DLL output
doc/                          # Additional documents
```

`Include/TinySECSGemDLL/` is generated from the source headers during the DLL post-build step. If you edit public headers, edit the source copy under `TinySECSGemDLL/` first.

------

## Dependencies

TinySECSGem depends on [HP-Socket](https://github.com/ldcsaa/HP-Socket) for TCP transport.

This repository keeps prebuilt HP-Socket static libraries for convenience:

| Platform | Configuration | Library |
| -------- | ------------- | ------- |
| x86      | Debug         | `third_party/Lib/HPSocket/x86/static/HPSocket_D.lib` |
| x86      | Release       | `third_party/Lib/HPSocket/x86/static/HPSocket.lib` |
| x64      | Debug         | `third_party/Lib/HPSocket/x64/static/HPSocket_D.lib` |
| x64      | Release       | `third_party/Lib/HPSocket/x64/static/HPSocket.lib` |

If you rebuild HP-Socket yourself, use the same Visual Studio toolset, platform target, and runtime library as TinySECSGem:

- Debug: `/MDd`
- Release: `/MD`

This is important because TinySECSGem links the HP-Socket **static** library into `TinySECSGemDLL`. When building HP-Socket, change the C/C++ Runtime Library option to `/MDd` for Debug and `/MD` for Release. Do not leave HP-Socket on `/MT` or `/MTd` when building the libraries used by this project.

Required preprocessor definition when linking HP-Socket statically:

```text
HPSOCKET_STATIC_LIB
```

HP-Socket is licensed under the Apache License 2.0. Because this repository includes HP-Socket headers and prebuilt libraries, releases should include a third-party notice file such as `THIRD_PARTY_NOTICES.md`.

------

## Supported Environment

- Windows 10 / Windows 11
- Visual Studio 2019 or later is recommended
- Visual Studio 2017 or Visual Studio 2015 can be used by retargeting the projects and rebuilding HP-Socket
- Default project toolset: v142
- Retargetable toolsets: v141 or v140
- Win32 / x64
- Debug / Release
- C++11 or later

------

## Build

1. Open `TinySECSGemDLL.sln` with Visual Studio 2019 or later, or retarget it for Visual Studio 2017 / 2015.
2. Select one of the supported solution configurations:
   - Debug | Win32
   - Release | Win32
   - Debug | x64
   - Release | x64
3. Build `TinySECSGemDLL` first.
4. Build `Test` after the DLL project succeeds.

### Retargeting to Visual Studio 2017 or 2015

The source code currently uses C++11-level language and standard library features, so the library can be built with older Visual Studio toolsets if the projects and HP-Socket dependency are kept consistent.

To build with an older toolset:

1. Change the `Platform Toolset` of both `TinySECSGemDLL` and `Test`:
   - Visual Studio 2017: `v141`
   - Visual Studio 2015: `v140`
2. Select an installed Windows SDK in `Windows SDK Version`.
3. Rebuild HP-Socket static libraries with the same toolset and platform:
   - `v141` or `v140`
   - Win32 or x64
   - Debug `/MDd`
   - Release `/MD`
4. Replace the HP-Socket libraries under `third_party/Lib/HPSocket/<arch>/static/`, or update the project library directories to point to your rebuilt libraries.
5. Build `TinySECSGemDLL` first, then build `Test`.

Do not mix toolsets. For example, if TinySECSGemDLL is built with `v141`, the HP-Socket static library linked into it should also be built with `v141`.

The DLL project defines:

```text
SECSGEMDLL_EXPORTS
HPSOCKET_STATIC_LIB
```

Build outputs:

| Project | Output |
| ------- | ------ |
| `TinySECSGemDLL` | `.lib`, `.dll`, `.pdb`, and `.exp` under `Lib/TinySECSGemDLL/x86/` or `Lib/TinySECSGemDLL/x64/` |
| `TinySECSGemDLL` post-build | Public headers copied to `Include/TinySECSGemDLL/` |
| `TinySECSGemDLL` post-build | Runtime DLL copied to `Bin/` |
| `Test` | `Test.exe` under `Bin/` |

Debug builds use the `_D` suffix:

- `TinySECSGemDLL_D.dll`
- `TinySECSGemDLL_D.lib`

Release builds use:

- `TinySECSGemDLL.dll`
- `TinySECSGemDLL.lib`

------

## Quick Start

The snippets below show the main usage pattern. For more complete usage examples, see the `Test` project, especially `Test/main.cpp`, which includes Passive mode, Active mode, message validation, primary message handling, secondary replies, and Item Tree construction/parsing.

### Passive Mode

```cpp
#include "SecsGemLibrary.h"

#include <stdio.h>
#include <string.h>

using namespace TinySECSGem;

class MyHandler : public ISecsGemHandler
{
public:
    void OnPrimaryMessage(const SECSMessage& msg) override
    {
        printf("Received S%dF%d\n", msg.GetStream(), msg.GetFunction());
    }
};

int main()
{
    MyHandler handler;
    SecsGem gem;

    SocketParameter param;
    param.m_mode = EnumHSMSMode::EnumHSMSMode_Passive;
    param.m_port = 5000;
    param.m_deviceId = 0;
    strcpy_s(param.m_ip, "0.0.0.0");

    gem.SetParameter(param);
    gem.SetHandler(&handler);

    if (!gem.Start())
        return 1;

    getchar();
    gem.Stop();
    return 0;
}
```

### Active Mode

```cpp
SocketParameter param;
param.m_mode = EnumHSMSMode::EnumHSMSMode_Active;
param.m_port = 5000;
param.m_deviceId = 0;
param.m_autoSelect = true;
param.m_autoReconnect = true;
strcpy_s(param.m_ip, "192.168.1.100");

gem.SetParameter(param);
gem.SetHandler(&handler);
gem.Start();
```

### Send a SECS-II Message

Send application SECS-II data messages after the HSMS session reaches `Selected`, for example after `OnSelected()` is called or when `gem.IsSelected()` returns true.

```cpp
gem.SendDataMessage(1, 1, true);

auto body = std::make_shared<ListItem>();
body->push_back(new ASCIIItem("LOT01"));
body->push_back(new UInt4ByteItem(100));

gem.SendDataMessage(2, 17, true, body);
```

### Build and Parse an Item Tree

```cpp
ListItem* list = new ListItem();
list->push_back(new ASCIIItem("Hello"));

Int4ByteItem* i4 = new Int4ByteItem();
i4->push_back(10);
i4->push_back(20);
list->push_back(i4);

std::cout << list->print() << std::endl;

if (auto* a = list->get<ASCIIItem>(0))
    std::cout << a->get() << std::endl;

delete list;
```

`ListItem::push_back()` transfers ownership of the raw child pointer to the list. Delete the parent list once you are done with the tree.

When an incoming SECS-II body is parsed, the library wraps the parsed body in a `TopItem` so application code can access the result as a single tree. `TopItem` is only a virtual root for parsed data and is not serialized as an actual SECS-II item.

### Send a Secondary Reply

In a real handler, keep a pointer or reference to the owning `SecsGem` instance before sending replies.

```cpp
class MyHandler : public ISecsGemHandler
{
public:
    explicit MyHandler(SecsGem& gem) : m_gem(gem) {}

    void OnPrimaryMessage(const SECSMessage& msg) override
    {
        if (msg.GetStream() == 1 && msg.GetFunction() == 1)
        {
            auto body = std::make_shared<ListItem>();
            body->push_back(new ASCIIItem("TinySECSGem"));
            body->push_back(new ASCIIItem(SecsGem::GetVersion()));

            SECSMessage reply;
            BlockHeader header;
            header.Set_DataMessage_GenSecondaryMsgHeader(msg.GetHead());
            reply.SetHead(header);
            reply.SetData(body);

            m_gem.SendSECSMessage(reply);
        }
    }

private:
    SecsGem& m_gem;
};
```

The handler and validator objects are not copied by `SecsGem`. Make sure objects passed to `SetHandler()` and `SetSECSMsgValidator()` remain alive while the `SecsGem` instance can call them.

For a fuller runnable reference, open the `Test` project in the solution and review `Test/main.cpp`.

------

## API Overview

| Type | Purpose |
| ---- | ------- |
| `SecsGem` | Main controller for configuration, lifecycle, HSMS control messages, and SECS-II data messages |
| `SocketParameter` | Connection settings such as mode, IP, port, device ID, timers, auto-select, and auto-reconnect. `m_maxConn` is currently limited to `1` for HSMS-SS single-session behavior. |
| `ISecsGemHandler` | Callback interface for connection events, selection events, messages, timeouts, and protocol errors |
| `ISECSMsgValidator` | Optional validation hook for stream, function, and message body checks |
| `SECSMessage` | Wrapper around the HSMS block header and optional SECS-II item body |
| `BlockHeader` | HSMS 10-byte message header model |
| `BaseItem` | Abstract base class for all SECS-II item types |
| `ListItem` | Ordered owning container for nested SECS-II items |
| `TopItem` | Virtual root node used only to represent parsed incoming message data as an Item Tree; it is not a real SECS-II wire item |

------

## Protocol Coverage

| Standard | Description | Current Status |
| -------- | ----------- | -------------- |
| SEMI E5  | SECS-II message content and item format | Supported |
| SEMI E37 | HSMS-SS transport/session layer | Supported |
| SEMI E30 | GEM equipment behavior model | Planned |

------

## Limitations and Known Notes

- This library currently focuses on the SECS communication layer.
- GEM behavior features from SEMI E30 are not implemented yet.
- The current transport/session implementation supports a maximum connection count of `1`, matching HSMS-SS single-session mode.
- `JIS8Item` and `UnicodeItem` are reserved but currently unsupported.
- SECS-II child items use owning raw pointer semantics; ownership transfers on `push_back`.
- Public headers are copied into `Include/TinySECSGemDLL/` by the Visual Studio post-build event.
- SML text parsing, message tracing, more examples, CMake support, and GEM features are roadmap items.

------

## Troubleshooting

| Symptom | Check |
| ------- | ----- |
| `HPSocket.lib` or `HPSocket_D.lib` not found | Confirm the selected platform matches `third_party/Lib/HPSocket/x86/static/` or `third_party/Lib/HPSocket/x64/static/`. |
| Link errors involving HP-Socket symbols | Confirm `HPSOCKET_STATIC_LIB` is defined and the correct Debug/Release HP-Socket library is selected. |
| Runtime library mismatch warnings or errors | Rebuild the HP-Socket static libraries with `/MDd` for Debug and `/MD` for Release, matching TinySECSGemDLL. Avoid `/MT` and `/MTd`. |
| `Test.exe` cannot find `TinySECSGemDLL.dll` | Build `TinySECSGemDLL` first and confirm the DLL was copied to `Bin/`. |
| Debug library not found | Debug output uses the `_D` suffix: `TinySECSGemDLL_D.lib` and `HPSocket_D.lib`. |
| Public headers look stale | Rebuild the DLL project so the post-build event copies headers to `Include/TinySECSGemDLL/`. |

------

## Version

Current version: **v1.0.0**

------

## Development Roadmap

- [x] SEMI E5 SECS-II data model
- [x] SEMI E37 HSMS-SS communication layer
- [x] Auto reconnect support
- [ ] SML text parser
- [ ] Message log tracing system
- [ ] More protocol examples
- [ ] SEMI E30 GEM state/event model
- [ ] CMake build support

------

## License

TinySECSGem is released under the MIT License.

This repository also includes HP-Socket headers and prebuilt static libraries. HP-Socket is licensed under the Apache License 2.0. Keep the corresponding third-party license and notice information with source and binary releases.
