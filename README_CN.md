[English](README.md)

# TinySECSGem

![platform](https://img.shields.io/badge/platform-Windows-blue)
![compiler](https://img.shields.io/badge/VisualStudio-2015%2B-green)
![standard](https://img.shields.io/badge/SEMI-E5%20%7C%20E37-orange)
![license](https://img.shields.io/badge/license-MIT-brightgreen)

TinySECSGem 是一个面向半导体设备端与上位机端的轻量级 Windows C++ SECS/HSMS 通信库。

当前版本聚焦于通信层：

- **SEMI E37 / HSMS-SS** 传输与会话行为
- **SEMI E5 / SECS-II** 消息序列化、解析与 Item Tree 数据模型

**SEMI E30** 中的 GEM 行为特性仍处于规划阶段，当前版本尚未实现。TinySECSGem 适合用于构建 SECS 消息收发、集成原型、Host 侧工具和设备侧通信服务，但不声明已经具备完整 GEM 设备行为模型。

------

## 项目状态与适用场景

TinySECSGem 适用于：

- HSMS-SS Active 或 Passive TCP 通信
- SECS-II 数据消息构建与解析
- 设备端或 Host 端 SECS 集成原型
- Visual Studio 项目中的 Windows DLL 集成
- 基于 Stream 9 错误回复的自定义应用层校验

它还不是完整 GEM 实现。GEM 状态/事件模型、Collection Event、Alarm、Report、SML 文本解析和完整生产设备行为属于后续路线图。

------

## 功能特性

### 传输层

- HSMS-SS Active 和 Passive 模式
- 基于 HP-Socket Pull 模式 API 的 TCP 传输
- 可配置 IP、端口、Device ID 和重连行为；当前最大连接数为 `1`，对应 HSMS-SS

### 会话行为

- HSMS 控制消息：
  - Select.req / Select.rsp
  - Deselect.req / Deselect.rsp
  - Linktest.req / Linktest.rsp
  - Separate.req
  - Reject.req
- HSMS 连接状态管理：
  - NotConnected
  - ConnectedNotSelected
  - Selected
- T3、T5、T6、T7、T8、T1 定时器
- 自动 Primary / Secondary 事务匹配
- 可选自动 Select 和自动重连

### 消息模型

- SECS-II 网络字节流序列化与解析
- 层级 Item Tree 消息体模型
- `TopItem` 只是虚拟根节点，用于把解析后的消息数据暴露成一棵树；它不是网络上传输的真实 SECS-II Item 格式
- 支持的 Item 类型：
  - ListItem / TopItem
  - ASCIIItem
  - BinaryItem
  - BooleanItem
  - Int1ByteItem / Int2ByteItem / Int4ByteItem / Int8ByteItem
  - UInt1ByteItem / UInt2ByteItem / UInt4ByteItem / UInt8ByteItem
  - Float4ByteItem / Float8ByteItem

### 校验与错误处理

- 自定义 `ISECSMsgValidator` 校验钩子
- 自动 Stream 9 错误回复：
  - S9F3：不支持的 Stream
  - S9F5：不支持的 Function
  - S9F7：非法消息数据
- 通过 `ISecsGemHandler` 接收协议错误与超时回调

------

## 仓库结构

```text
TinySECSGemDLL/               # 库源码和 Visual Studio 工程
Include/TinySECSGemDLL/       # 构建后复制出的公共头文件
Test/                         # 控制台演示程序和集成示例
third_party/Include/HPSocket/ # HP-Socket 头文件
third_party/Lib/HPSocket/     # HP-Socket 预编译静态库
Bin/                          # 构建复制出的运行时输出
Lib/                          # TinySECSGemDLL 导入库和 DLL 输出
doc/                          # 附加文档
```

`Include/TinySECSGemDLL/` 由 DLL 工程的 post-build 步骤从源码头文件复制生成。如果需要修改公共头文件，请优先修改 `TinySECSGemDLL/` 下的源文件。

------

## 依赖项

TinySECSGem 使用 [HP-Socket](https://github.com/ldcsaa/HP-Socket) 作为 TCP 传输层。

本仓库为了便于直接构建，保留了 HP-Socket 预编译静态库：

| 平台 | 配置 | 库文件 |
| ---- | ---- | ------ |
| x86  | Debug   | `third_party/Lib/HPSocket/x86/static/HPSocket_D.lib` |
| x86  | Release | `third_party/Lib/HPSocket/x86/static/HPSocket.lib` |
| x64  | Debug   | `third_party/Lib/HPSocket/x64/static/HPSocket_D.lib` |
| x64  | Release | `third_party/Lib/HPSocket/x64/static/HPSocket.lib` |

如果你自行重新编译 HP-Socket，请确保它与 TinySECSGem 使用相同的 Visual Studio 工具集、平台目标和运行时库：

- Debug：`/MDd`
- Release：`/MD`

这一点很重要，因为 TinySECSGem 会把 HP-Socket 的**静态库**链接进最终生成的 `TinySECSGemDLL`。编译 HP-Socket 时，需要在 C/C++ Runtime Library 选项中把 Debug 改为 `/MDd`，Release 改为 `/MD`。用于本项目的 HP-Socket 静态库不要保留为 `/MT` 或 `/MTd`。

静态链接 HP-Socket 时必须定义：

```text
HPSOCKET_STATIC_LIB
```

HP-Socket 使用 Apache License 2.0。由于本仓库包含 HP-Socket 头文件和预编译库，发布源码或二进制包时应随附第三方声明文件，例如 `THIRD_PARTY_NOTICES.md`。

------

## 支持环境

- Windows 10 / Windows 11
- 推荐使用 Visual Studio 2019 或更高版本
- 通过重新定向工程并重新编译 HP-Socket，也可以使用 Visual Studio 2017 或 Visual Studio 2015
- 默认工程工具集：v142
- 可重新定向工具集：v141 或 v140
- Win32 / x64
- Debug / Release
- C++11 或更高版本

------

## 编译方式

1. 使用 Visual Studio 2019 或更高版本打开 `TinySECSGemDLL.sln`，或者将工程重新定向到 Visual Studio 2017 / 2015。
2. 选择一个支持的解决方案配置：
   - Debug | Win32
   - Release | Win32
   - Debug | x64
   - Release | x64
3. 先构建 `TinySECSGemDLL`。
4. DLL 工程构建成功后，再构建 `Test`。

### 重新定向到 Visual Studio 2017 或 2015

当前源码主要使用 C++11 级别的语言和标准库特性，因此只要工程和 HP-Socket 依赖保持一致，就可以使用更低版本的 Visual Studio 工具集编译。

使用较低工具集编译时：

1. 修改 `TinySECSGemDLL` 和 `Test` 两个工程的 `Platform Toolset`：
   - Visual Studio 2017：`v141`
   - Visual Studio 2015：`v140`
2. 在 `Windows SDK Version` 中选择本机已安装的 Windows SDK。
3. 使用相同工具集和平台重新编译 HP-Socket 静态库：
   - `v141` 或 `v140`
   - Win32 或 x64
   - Debug `/MDd`
   - Release `/MD`
4. 将重新编译得到的 HP-Socket 库替换到 `third_party/Lib/HPSocket/<arch>/static/`，或修改工程的库目录指向你的新库。
5. 先构建 `TinySECSGemDLL`，再构建 `Test`。

不要混用工具集。例如 TinySECSGemDLL 如果使用 `v141` 编译，那么链接进它的 HP-Socket 静态库也应使用 `v141` 编译。

DLL 工程会定义：

```text
SECSGEMDLL_EXPORTS
HPSOCKET_STATIC_LIB
```

构建输出：

| 工程 | 输出 |
| ---- | ---- |
| `TinySECSGemDLL` | `.lib`、`.dll`、`.pdb`、`.exp` 输出到 `Lib/TinySECSGemDLL/x86/` 或 `Lib/TinySECSGemDLL/x64/` |
| `TinySECSGemDLL` post-build | 公共头文件复制到 `Include/TinySECSGemDLL/` |
| `TinySECSGemDLL` post-build | 运行时 DLL 复制到 `Bin/` |
| `Test` | `Test.exe` 输出到 `Bin/` |

Debug 构建使用 `_D` 后缀：

- `TinySECSGemDLL_D.dll`
- `TinySECSGemDLL_D.lib`

Release 构建使用：

- `TinySECSGemDLL.dll`
- `TinySECSGemDLL.lib`

------

## 快速开始

下面的代码片段展示主要使用方式。更完整的使用案例请参考 `Test` 项目，尤其是 `Test/main.cpp`，其中包含 Passive 模式、Active 模式、消息校验、Primary 消息处理、Secondary 回复以及 Item Tree 构建/解析示例。

### Passive 模式

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

### Active 模式

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

### 发送 SECS-II 消息

应用层 SECS-II 数据消息应在 HSMS 会话进入 `Selected` 后发送，例如收到 `OnSelected()` 回调后，或 `gem.IsSelected()` 返回 true 后。

```cpp
gem.SendDataMessage(1, 1, true);

auto body = std::make_shared<ListItem>();
body->push_back(new ASCIIItem("LOT01"));
body->push_back(new UInt4ByteItem(100));

gem.SendDataMessage(2, 17, true, body);
```

### 构建与解析 Item Tree

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

`ListItem::push_back()` 会把原始子项指针的所有权转移给 List。使用完 Item Tree 后，删除父级 List 即可。

当库解析入站 SECS-II 消息体时，会用 `TopItem` 包装解析结果，方便应用层把数据当作一棵完整的 Item Tree 访问。`TopItem` 只是解析后的虚拟根节点，不会作为真实 SECS-II Item 被序列化到网络字节流中。

### 发送 Secondary 回复

在实际 handler 中，可以保存所属 `SecsGem` 实例的指针或引用，用于发送回复。

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

`SecsGem` 不会复制传入的 handler 和 validator 对象。传给 `SetHandler()` 和 `SetSECSMsgValidator()` 的对象必须在 `SecsGem` 可能调用它们期间保持有效。

如果需要更完整、可运行的参考，请在解决方案中打开 `Test` 项目并查看 `Test/main.cpp`。

------

## API 概览

| 类型 | 作用 |
| ---- | ---- |
| `SecsGem` | 主控制器，负责配置、生命周期、HSMS 控制消息和 SECS-II 数据消息 |
| `SocketParameter` | 连接参数，包括模式、IP、端口、Device ID、定时器、自动 Select 和自动重连。`m_maxConn` 当前限制为 `1`，对应 HSMS-SS 单会话行为。 |
| `ISecsGemHandler` | 连接事件、Selected 事件、消息、超时和协议错误回调接口 |
| `ISECSMsgValidator` | 可选校验钩子，用于检查 Stream、Function 和消息体 |
| `SECSMessage` | HSMS Block Header 和可选 SECS-II Item Body 的消息封装 |
| `BlockHeader` | HSMS 10 字节消息头模型 |
| `BaseItem` | 所有 SECS-II Item 类型的抽象基类 |
| `ListItem` | 有序、拥有子项所有权的 SECS-II 嵌套容器 |
| `TopItem` | 仅用于把已解析入站消息数据表示为 Item Tree 的虚拟根节点；它不是真实的 SECS-II 网络传输 Item |

------

## 协议覆盖范围

| 标准 | 描述 | 当前状态 |
| ---- | ---- | -------- |
| SEMI E5  | SECS-II 消息内容与 Item 格式 | 已支持 |
| SEMI E37 | HSMS-SS 传输/会话层 | 已支持 |
| SEMI E30 | GEM 设备行为模型 | 规划中 |

------

## 限制与已知说明

- 当前库主要聚焦于 SECS 通信层。
- SEMI E30 中的 GEM 行为特性尚未实现。
- 当前传输/会话实现最大连接数只支持 `1`，对应 HSMS-SS 单会话模式。
- `JIS8Item` 和 `UnicodeItem` 为预留类型，当前暂不支持。
- SECS-II 子 Item 使用拥有所有权的原始指针语义；`push_back` 时转移所有权。
- 公共头文件由 Visual Studio post-build 事件复制到 `Include/TinySECSGemDLL/`。
- SML 文本解析、消息追踪、更多示例、CMake 支持和 GEM 功能均属于路线图项目。

------

## 常见问题排查

| 现象 | 检查项 |
| ---- | ------ |
| 找不到 `HPSocket.lib` 或 `HPSocket_D.lib` | 确认当前平台与 `third_party/Lib/HPSocket/x86/static/` 或 `third_party/Lib/HPSocket/x64/static/` 匹配。 |
| 出现 HP-Socket 符号链接错误 | 确认已定义 `HPSOCKET_STATIC_LIB`，并选择了正确的 Debug/Release HP-Socket 库。 |
| 出现运行时库不匹配警告或错误 | 使用 `/MDd` 重新编译 Debug 版 HP-Socket 静态库，使用 `/MD` 重新编译 Release 版 HP-Socket 静态库，并与 TinySECSGemDLL 保持一致；避免 `/MT` 和 `/MTd`。 |
| `Test.exe` 找不到 `TinySECSGemDLL.dll` | 先构建 `TinySECSGemDLL`，并确认 DLL 已复制到 `Bin/`。 |
| 找不到 Debug 库 | Debug 输出使用 `_D` 后缀：`TinySECSGemDLL_D.lib` 和 `HPSocket_D.lib`。 |
| 公共头文件不是最新 | 重新构建 DLL 工程，让 post-build 事件复制头文件到 `Include/TinySECSGemDLL/`。 |

------

## 当前版本

当前版本：**v1.0.0**

------

## 开发路线图

- [x] SEMI E5 SECS-II 数据模型
- [x] SEMI E37 HSMS-SS 通信层
- [x] 自动重连支持
- [ ] SML 文本解析器
- [ ] 消息日志追踪系统
- [ ] 更多协议示例
- [ ] SEMI E30 GEM 状态/事件模型
- [ ] CMake 构建支持

------

## 开源协议

TinySECSGem 使用 MIT License 发布。

本仓库同时包含 HP-Socket 头文件和预编译静态库。HP-Socket 使用 Apache License 2.0。发布源码或二进制包时，请保留相应的第三方许可证和声明信息。
