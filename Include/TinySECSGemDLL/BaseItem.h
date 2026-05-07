#pragma once
#include<vector>
#include<string>
#include<cstdint>
#include "TinySECSGem.h"

namespace TinySECSGem 
{
	class BinaryItem;
	class BooleanItem;
	class ASCIIItem;
	class UnicodeItem;
	class JIS8Item;
	class Int1ByteItem;
	class Int2ByteItem;
	class Int4ByteItem;
	class Int8ByteItem;
	class UInt1ByteItem;
	class UInt2ByteItem;
	class UInt4ByteItem;
	class UInt8ByteItem;
	class Float4ByteItem;
	class Float8ByteItem;
	class ListItem;


    /**
    * @brief SECS-II数据项抽象基类
    *
    * 该类定义了SECS/GEM协议中所有SECS-II数据项的统一抽象接口，
    * 用于表示标准SECS数据结构中的任意数据节点，包括基础类型、
    * 数组类型以及复合结构（LIST）类型等。
    *
    * 所有具体SECS-II数据类型（如ASCII、U1、U2、LIST等）均继承自该类实现。
    */
    class TinySECSGEM_API BaseItem
    {
    protected:
        EnumSECSItemType m_eItemType;   ///< SECS-II数据类型标识（如ASCII/U1/U2/LIST等）

    public:

        /**
         * @brief 构造BaseItem对象
         */
        BaseItem();

        /**
         * @brief 析构BaseItem对象
         */
        virtual ~BaseItem();

        /**
         * @brief 获取当前数据项序列化后的总字节大小
         * @return 当前数据项占用的字节数（包含子节点）
         */
        virtual size_t totalByteSize() const = 0;

        /**
         * @brief 获取当前数据项数组元素数量
         * @return 数组元素个数
         */
        virtual size_t GetArraySize() const = 0;

        /**
         * @brief 清空当前数据内容
         */
        virtual void clear() = 0;

        /**
         * @brief 深拷贝当前数据项对象
         * @return 返回新创建的对象副本（调用方负责释放内存）
         */
        virtual BaseItem* clone() = 0;

        /**
         * @brief 将当前数据结构转换为可读字符串（调试用途）
         * @param frontStr 输出前缀（用于缩进格式化）
         * @return 格式化后的字符串表示
         */
        virtual std::string print(std::string frontStr = "") const = 0;

        /**
         * @brief 序列化当前数据项为SECS-II字节流
         * @param buff 输出缓冲区
         * @param buffSize 缓冲区大小
         * @return 实际写入的字节数
         */
        virtual int Serialization(BYTE* buff, size_t buffSize) = 0;

        /**
         * @brief 从SECS-II字节流反序列化数据项
         * @param startBufPos 起始缓冲区指针
         * @param endBufPos 结束缓冲区指针（buf不包括endBufPos）
         * @param bOk 输出参数，表示解析是否成功
         * @return 解析后的BaseItem对象指针（失败返回nullptr）
         */
        static BaseItem* Deserialization(BYTE* startBufPos, BYTE* endBufPos, bool& bOk);

        /**
         * @brief 解析SECS-II数据项头部信息
         *
         * 用于解析数据项格式类型及长度信息（Item Header）
         *
         * @param bufStart 起始缓冲区指针
         * @param bufEnd 结束缓冲区指针（buf不包括bufEnd位置）
         * @param format 输出数据类型格式
         * @param lengthByteSize 输出长度字段所占字节数
         * @param lengthByte 输出数据长度值
         * @return true 解析成功
         * @return false 解析失败
         */
        static bool DeserializationItemHeader(
            BYTE* bufStart,
            BYTE* bufEnd,
            EnumSECSItemType& format,
            size_t& lengthByteSize,
            size_t& lengthByte);

        /**
         * @brief 序列化SECS-II数据项头部信息
         *
         * 将数据类型及长度信息编码为SECS-II Item Header格式
         *
         * @param bufStart 输出缓冲区起始位置
         * @param bufEnd 输出缓冲区结束位置（buf不包括BufEnd）
         * @param format 数据类型格式
         * @param singleTypeSize 单个元素大小
         * @param arraySize 数组元素数量
         * @return 写入的字节数
         */
        static int SerializationItemHeader(
            BYTE* bufStart,
            BYTE* bufEnd,
            EnumSECSItemType format,
            size_t singleTypeSize,
            size_t arraySize);

        /**
         * @brief 获取当前数据项格式类型
         * @return EnumSECSItemType 当前SECS-II数据类型
         */
        EnumSECSItemType getFormat() const;

        /**
         * @brief 类型安全转换为指定派生类型
         * @tparam T 目标类型
         * @return 转换成功返回指针，否则返回nullptr
         */
        template<typename T>
        T* as() {
            return dynamic_cast<T*>(this);
        }

        /**
         * @brief const版本类型安全转换
         */
        template<typename T>
        const T* as() const {
            return dynamic_cast<const T*>(this);
        }

        /**
         * @brief 判断当前对象是否为指定类型
         * @tparam T 目标类型
         * @return true 类型匹配
         * @return false 类型不匹配
         */
        template<typename T>
        bool is() const {
            return dynamic_cast<const T*>(this) != nullptr;
        }
    };

}



