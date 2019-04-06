/*************************************************************************
	> File Name: BitWriteStream.h
	> Author: amoscykl
	> Mail: amoscykl@163.com 
	> Created Time: 2019年03月23日 星期六 22时03分55秒
 ************************************************************************/

#include<iostream>
#include<assert.h>
#include<string.h>
using namespace std;


class BitWriteStream
{
	private:
		char*_pos;
		int _size;
		char* _start;
		void check(int n)
		{
			assert(_size >= (_pos - _start));
		}
	public:
		BitWriteStream(char* data, int len)
		{
			assert(data != NULL);
			assert(len > 0);
			_pos = _start = data;
			_size = len;
		}

		~BitWriteStream()
		{
		}
		
		void write_u8(uint8_t n)
		{
			check(1);
			*_pos = n;
			_pos += 1;
			return;
		}

		void write_u16(uint16_t n)
		{
			check(2);
			*_pos = n >> 8;
			_pos ++;
			*_pos = n;
			_pos ++;
			return;
		}
		
		void write_u24(uint32_t n)
		{
			check(3);
			*_pos = n >> 16;
			*(_pos + 1) = n >> 8;
			*(_pos + 2) = n;
			_pos += 3;
			return ;
		}

		void write_u32(uint32_t n)
		{
			check(4);
			*_pos = n >> 24;
			*(_pos + 1) = n >> 16;
			*(_pos + 2) = n >> 8;
			*(_pos + 3) = n;
			_pos += 4;
			return ;
		}

		void write_char(char* data, int len)
		{
			check(len);
			memcpy(_pos,  data, len);
			_pos += len;
			return;
		}

		int get_able_len()
		{
			return _size - (_pos - _start);
		}

		int get_len()
		{
			return _size;
		}

};
