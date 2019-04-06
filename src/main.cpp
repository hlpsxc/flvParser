#include <iostream>
#include "BitReadStream.h"
#include "BitWriteStream.h"
#include "FlvParser.h"
void test_read()
{
	char data[] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01};
	BitReadStream bs(data, 10);
	std::cout << (uint32_t)bs.get_u8() << "," << bs.get_u16() << "," << bs.get_u24() << "," << bs.get_u32() << std::endl;
}

void test_write()
{
	char* data = new char[10];
	BitWriteStream bws(data, 10);
	bws.write_u8(1);
	bws.write_u16(1);
	bws.write_u24(1);
	bws.write_u32(1);
	std::cout << bws.get_able_len() << "," << bws.get_len() << std::endl;
}

int main()
{
	test_read();
    std::cout << "hello world" << std::endl;
	test_write();
	FlvParser parser("/home/skl/pcode/video/dp.flv", "/home/skl/pcode/video/dp.h264", "/home/skl/pcode/video/dp.aac");
	while(1)
	{
		if(parser.read_next_tag() == false)
		{
			break;
		}

	}
    return 0;
}
