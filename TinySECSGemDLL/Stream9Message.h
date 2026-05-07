#pragma once

#include"SecsMessage.h"
#include<string>

namespace TinySECSGem
{
    /**
    * @brief SECS/GEM Stream 9异常消息生成器
    *
    * 该类用于生成SECS-II协议标准中定义的Stream 9错误报告消息，
    * 当通信过程中发生设备ID错误、Stream错误、Function错误、
    * 消息格式错误、事务超时、数据长度异常或会话超时等协议异常时，
    * 可通过本类快速构建对应的S9Fx标准异常响应消息。
    *
    * 所有接口均为静态工厂函数，无需实例化对象即可直接调用。
    */
    class TinySECSGEM_API Stream9Message
    {
    public:

        /**
         * @brief 生成S9F1未识别设备ID异常消息
         *
         * 当接收到的消息头中Device ID无法识别时调用。
         *
         * @param mheader 原始接收消息头
         * @return SECSMessage 构造完成的S9F1异常消息
         */
        static SECSMessage GenerateS9F1Message(BlockHeader mheader);

        /**
         * @brief 生成S9F3未识别Stream异常消息
         *
         * 当接收到未知Stream编号的消息时调用。
         *
         * @param mheader 原始接收消息头
         * @return SECSMessage 构造完成的S9F3异常消息
         */
        static SECSMessage GenerateS9F3Message(BlockHeader mheader);

        /**
         * @brief 生成S9F5未识别Function异常消息
         *
         * 当接收到未知Function编号的消息时调用。
         *
         * @param mheader 原始接收消息头
         * @return SECSMessage 构造完成的S9F5异常消息
         */
        static SECSMessage GenerateS9F5Message(BlockHeader mheader);

        /**
         * @brief 生成S9F7无效数据格式异常消息
         *
         * 当接收到格式错误或无法解析的数据消息时调用。
         *
         * @param mheader 原始接收消息头
         * @return SECSMessage 构造完成的S9F7异常消息
         */
        static SECSMessage GenerateS9F7Message(BlockHeader mheader);

        /**
         * @brief 生成S9F9事务超时异常消息
         *
         * 当事务请求在规定时间内未收到响应时调用。
         *
         * @param mheader 原始事务消息头
         * @return SECSMessage 构造完成的S9F9异常消息
         */
        static SECSMessage GenerateS9F9Message(BlockHeader mheader);

        /**
         * @brief 生成S9F11数据过长异常消息
         *
         * 当接收到超出协议允许长度范围的数据消息时调用。
         *
         * @param mheader 原始接收消息头
         * @return SECSMessage 构造完成的S9F11异常消息
         */
        static SECSMessage GenerateS9F11Message(BlockHeader mheader);

        /**
         * @brief 生成S9F13会话超时异常消息
         *
         * 当当前HSMS会话超过允许保持时间或会话状态异常失效时调用。
         *
         * @param mheader 原始消息头
         * @param mexp message expected in form of SxxFyy
         * @param edid expected data identification, PPID or SPID or PTN
         * @return SECSMessage 构造完成的S9F13异常消息
         */
        static SECSMessage GenerateS9F13Message(BlockHeader mheader, std::string mexp, std::string edid);
    };

}


