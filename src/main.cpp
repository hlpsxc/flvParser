#include <iostream>
#include "BitReadStream.h"
#include "BitWriteStream.h"
#include "FlvParser.h"

void test_read() {
    char data[] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01};
    BitReadStream bs(data, 10);
    std::cout << (uint32_t) bs.get_u8() << "," << bs.get_u16() << "," << bs.get_u24() << "," << bs.get_u32()
              << std::endl;
}

void test_write() {
    char *data = new char[10];
    BitWriteStream bws(data, 10);
    bws.write_u8(1);
    bws.write_u16(1);
    bws.write_u24(1);
    bws.write_u32(1);
    std::cout << bws.get_able_len() << "," << bws.get_len() << std::endl;
}

int main(int argc, char **args) {
    if (argc < 2) {
        std::cout << "please input file" << std::endl;
        return 0;
    }
    char *input_file = args[1];
    char *out_video = NULL;
    char *out_audio = NULL;
    if (argc == 3) {
        out_video = args[2];
    } else if (argc == 4) {
        out_video = args[2];
        out_audio = args[3];
    }
    test_read();
    std::cout << "hello world" << std::endl;
    test_write();
    FlvParser parser(input_file, out_video, out_audio);
    while (1) {
        if (parser.read_next_tag() == false) {
            break;
        }

    }
    return 0;
}
