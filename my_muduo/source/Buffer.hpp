#pragma once

#include <vector>
#include <assert.h>
#include <cstdint>
#include <string>
#include <cstring>
#include "Log.hpp"

namespace muduo
{
    class Buffer
    {
        #define DEFAULT_SIZE_BUFFER 1024
    private:
        std::vector<char> buffer;  //内部是一段连续的空间
        uint64_t _write_idx;  // 写入位置
        uint64_t _read_indx;  // 读取位置


    public:
        Buffer():_write_idx(0),_read_indx(0),buffer(DEFAULT_SIZE_BUFFER) {}
        char *Begin() { return &(*buffer.begin()); }  //获取该连续空间的起始地址
        char *WritePostion() { return (Begin() + _write_idx); }
        char *ReadPostion() { return (Begin() + _read_indx); }

        const char *Begin() const { return (&(*buffer.begin())); }  //获取该连续空间的起始地址

        uint64_t TailIdleSize() { return buffer.size() - _write_idx; }
        uint64_t HeadIdleSize() { return _read_indx; }
        uint64_t ReadAbleSize() { return _write_idx - _read_indx; }

        void MoveReadOffset(uint64_t len) 
        {
            if(len == 0)
                return;
            assert(len <= ReadAbleSize());
            _read_indx += len;
        }

        void MoveWriteOffset(uint64_t len)
        {
            assert( len <= TailIdleSize() );
            _write_idx += len;
        }

        void EnsureWriteSpace(uint64_t len)
        {
            if(_write_idx + len < buffer.size())
            {
                return;
            }
            else if (TailIdleSize() + HeadIdleSize() >= len)
            {
                //移动数据
                uint64_t rsz = ReadAbleSize();
                std::copy(ReadPostion(), ReadPostion() + rsz, Begin());

                _read_indx = 0;
                _write_idx = rsz;
            }
            else if(TailIdleSize() + HeadIdleSize() < len)
            {
                //扩容
                buffer.resize(_write_idx + len);
            }
        }

        void Write(const void* data ,uint64_t len)
        {
            if(len == 0)
                return;
            EnsureWriteSpace(len);
            const char *d = (const char *)data;
            std::copy(d, d + len, WritePostion());
        }

        void WriteAndPush(const void* data ,uint64_t len)
        {
            Write(data, len);
            MoveWriteOffset(len);
        }

        void WriteString(const std::string& data)
        {
            Write(data.c_str(), data.size());
        }

        void WriteAndPushString(const std::string& data)
        {
            WriteString(data);
            MoveWriteOffset(data.size());
        }

        void WriteBuffer(Buffer& buf)
        {
            Write(buf.ReadPostion(),buf.ReadAbleSize());
        }

        void WriteAndPushBuffer(Buffer& buf)
        {
            WriteBuffer(buf);
            MoveWriteOffset(buf.ReadAbleSize());
        }

        void Read(void* buf,uint64_t len)
        {
            assert(len <= ReadAbleSize());
            std::copy(ReadPostion(), ReadPostion() + len, (char *)buf);
        }

        void ReadAndPop(void* buf,uint64_t len)
        {
            Read(buf, len);

            //偏移量移动
            MoveReadOffset(len);
        }

        std::string ReadAsString(uint64_t len)
        {
            assert(len <= ReadAbleSize());
            std::string str;
            str.resize(len);
            Read(&(str[0]), len);  // c_str 返回值是const类型，不允许修改

            return str;
        }

        std::string ReadAsStringAndPop(uint64_t len)
        {
            std::string str = ReadAsString(len);
            MoveReadOffset(len);
            return str;
        }

        char* FindCRLF()
        {
            void *res = memchr(ReadPostion(), '\n', ReadAbleSize());  //找 换行字符
            return (char*)res;
        }

        //读取一行数据
        std::string GetLine()
        {
            char *res = FindCRLF();
            if(res == NULL)
                return "";

            return ReadAsString(res - ReadPostion() + 1);
        }

        std::string GetLineAndPop()
        {
            std::string str = GetLine();
            MoveReadOffset(str.size());
            std::string _str("birthday");

            // DEG_LOG("GetLineAndPop  %d: %s",20041221,_str.c_str());
            INF_LOG("GetLineAndPop  %d: %s",20041221,_str.c_str());

            return str;
        }

        void Clear()
        {
            _write_idx = 0;
            _read_indx = 0;
        }
    };
}