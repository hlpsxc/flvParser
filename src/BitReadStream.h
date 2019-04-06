/*************************************************************************
	> File Name: BitReadStream.h
	> Author: amoscykl
	> Mail: amoscykl@163.com 
	> Created Time: 2019年03月21日 星期四 23时25分59秒
 ************************************************************************/

#include<iostream>
#include<assert.h>
using namespace std;

class BitReadStream
{
	private:
		char* _pos;
		char* _start;
		int _size;
	public:
		BitReadStream(const char* data, int len)
		{
			assert(data != NULL);
			assert(len >= 0);
			_start = _pos = const_cast<char*>(data);
			_size = len;
		}

		BitReadStream(const void* data, int len)
		{
			assert(data != NULL);
			assert(len >= 0);
			_start = _pos = reinterpret_cast<char*>(const_cast<void*>(data));
			_size = len;
		}

		~BitReadStream()
		{
		}

		uint8_t get_u8()
		{
			check(1);
			uint8_t ret = static_cast<uint8_t>(*_pos);
			_pos ++;
			return ret;
		}

		uint16_t get_u16()
		{
			check(2);
			uint16_t ret = (*(_pos) << 8) | *(_pos + 1);
			_pos += 2;
			return ret;
		}

		uint32_t get_u24()
		{
			check(3);
			uint32_t ret = (*(_pos) << 16) | (*(_pos + 1) << 8) | (*(_pos + 2));
			_pos += 3;
			return ret;
		}

		uint32_t get_u32()
		{
			check(4);
			uint32_t ret = (*(_pos) << 24) | (*(_pos + 1) << 16) | (*(_pos + 2) << 8) | (*(_pos + 3));
			_pos += 4;
			return ret;
		}

		char* get_char(int n)
		{
			check(n);
			char* ret = _pos;
			_pos += n;
			return ret;
		}

		int get_size()
		{
			return _size - (_pos - _start);
		}

	private:
		void check(int n)
		{
			assert(n <= get_size());
		}

};
