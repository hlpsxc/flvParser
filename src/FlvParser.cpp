//
// Created by skl on 19-3-23.
//

#include "FlvParser.h"
#include <assert.h>

static char H264_START_CODE[] = {0x00, 0x00, 0x00, 0x01};
#define H264_START_CODE_LENGTH 4

FlvParser::FlvParser(const char *file_name)
        : _file_name(file_name), _is_flv(false)
{
    _file = fopen(_file_name.c_str(), "rb");
    if (_file == NULL) {
        std::cout << "file open faild, filename:" << _file_name << ", err:" << errno << std::endl;
        return;
    }
    if (read_header()) {
        _is_flv = true;
        std::cout << "read flv header success" << std::endl;
    }
}

FlvParser::FlvParser(const char *file_name, const char *out_video_file_name, const char *out_audio_file_name)
        : _file_name(file_name), _is_flv(false), _out_video_file_name(out_video_file_name),
          _out_audio_file_name(out_audio_file_name)
{
    _file = fopen(_file_name.c_str(), "rb");
    if (_file == NULL) {
        std::cout << "file open faild, filename:" << _file_name << ", err:" << errno << std::endl;
        return;
    }
    if (read_header()) {
        _is_flv = true;
        std::cout << "read flv header success" << std::endl;
    }


    if (!_out_audio_file_name.empty()) {
        _out_audio_file = fopen(_out_audio_file_name.c_str(), "wb");
        if (_out_audio_file == NULL) {
            std::cout << "out audio file open faild, filename:" << _out_audio_file_name << ", err:" << errno
                      << std::endl;
        }
    }

    if (!_out_video_file_name.empty()) {
        _out_video_file = fopen(_out_video_file_name.c_str(), "wb");
        if (_out_video_file == NULL) {
            std::cout << "out video file open faild, filename:" << _out_video_file_name << ", err:" << errno
                      << std::endl;
        }
    }
}

FlvParser::~FlvParser()
{
    if (_file) {
        fclose(_file);
        _file = NULL;
    }
    if (_out_audio_file) {
        fclose(_out_audio_file);
        _out_audio_file = NULL;
    }
    if (_out_video_file) {
        fclose(_out_video_file);
        _out_video_file = NULL;
    }
}

bool FlvParser::read_header()
{
    char header[10];
    int ret = fread(header, 1, FLV_HEADER_DEFAULT_SIZE, _file);
    if (ret < 9) {
        std::cout << "file size < 9, not flv file" << std::endl;
        return false;
    }

    //FLV
    if (header[0] != 0x46 || header[1] != 0x4c || header[2] != 0x56) {
        std::cout << "not flv file" << std::endl;
        return false;
    }

    _header.version = header[3];//version
    _header.is_audio = 0x04 & header[4];
    _header.is_video = 0x01 & header[4];

    _header.header_size = (header[5] << 24) | (header[6] << 16) | (header[7] << 8) | header[8];
    if (_header.header_size != FLV_HEADER_DEFAULT_SIZE) {
        std::cout << "has ext header" << std::endl;
    }
    //处理扩展字段
    std::cout << "version:" << _header.version << ", hasVideo:" << _header.is_video << ", hasAudio:" << _header.is_audio
              << ", size:" << _header.header_size << std::endl;
    return true;
}

bool FlvParser::read_next_tag()
{
    assert(_is_flv == true);

    uint8_t data[12];
    int ret = fread(data, 1, 4, _file);
    if (ret != 4) {
        std::cout << "read pre tag size faild" << std::endl;
        _is_flv = false;
        return false;
    }

    ret = fread(data, 1, FLV_TAG_HEADER_DEFAULT_SIZE, _file);
    if (ret != FLV_TAG_HEADER_DEFAULT_SIZE) {
        std::cout << "read tag faild" << std::endl;
        _is_flv = false;
        return false;
    }

    TagHeader tag_header;
    tag_header.type = 0x1f & data[0];
    tag_header.data_size = (data[1] << 16) | (data[2] << 8) | (data[3]); //size
    tag_header.stamp = (data[4] << 16) | (data[5] << 8) | (data[6]); //stamp
    if (data[7] > 0) {
        tag_header.stamp |= (data[7] << 24);
    }
    tag_header.string_id = (data[8] << 16) | (data[9] << 8) | (data[10]); //string id;
    std::cout << "tag type:" << tag_header.type << ",dts:" << tag_header.stamp << ", size:"
              << tag_header.data_size << ",id:" << tag_header.string_id << std::endl;


    uint8_t *pTagData = new uint8_t[tag_header.data_size + 1];
    ret = fread(pTagData, 1, tag_header.data_size, _file);
    if (ret < tag_header.data_size) {
        std::cout << "bad file" << std::endl;
        _is_flv = false;
        return false;
    }

    if (tag_header.type == FLV_TAG_TYPE_AUDIO) {
        read_audio_tag(tag_header, pTagData, tag_header.data_size);
    } else if (tag_header.type == FLV_TAG_TYPE_VIDEO) {
        read_video_tag(tag_header, pTagData, tag_header.data_size);
    } else if (tag_header.type == FLV_TAG_TYPE_METADATA) {

    } else {

    }
    //handle
    delete[] pTagData;
    return true;
}

bool FlvParser::read_video_tag(TagHeader header, uint8_t *data, int len)
{
    uint8_t codec_type = data[0] & 0x0f;
    uint8_t frame_type = data[0] & 0xf0;

    if (FLV_VIDEO_CODEC_H264 != codec_type && FLV_VIDEO_CODEC_HEVE != codec_type) {
        std::cout << "no support video codec,id:" << codec_type << std::endl;
        return true;
    }

    uint8_t packet_type = data[1];

    int32_t cts = ((data[2] << 16) | (data[3] << 8) | (data[4]) + 0xff800000) ^0xff800000;
    int pts = header.stamp + cts;

    std::cout << "codec_type:" << (int) codec_type << ", frame_type:" << (int) frame_type << ",packet_type:"
              << (int) packet_type << ",pts:" << pts << std::endl;
    //avcDecorderConfigurationRecord
    if (packet_type == 0) {
        //265
        if (codec_type == FLV_VIDEO_CODEC_HEVE) {

            int index = 5;
            index += 1;//configrationversion
            index += 1;//general_profile_space, general_tier_flag, general_profile_idc
            index += 4;//general_profile_compatibility_flags
            index += 2; //general_constraint_indicator_flags
            index += 4;//
            index += 1;
            index += 2;
            index += 1;
            index += 1;
            index += 1;
            index += 1;
            index += 2;


            int8_t lenght_size_minus_one = data[index];
            index ++;
            lenght_size_minus_one &= 0x03;
            int nul_unit_length = lenght_size_minus_one;
            if(nul_unit_length == 2) {
                std::cout << "hevc nal lengthSizeMinusOne should nerver be 2" << std::endl;
                return true;
            }
            uint8_t num_of_arrays = data[index++];
            for(int i = 0; i < num_of_arrays; i++) {
                uint8_t unit_type = data[index++];
                uint64_t num_nalus = data[index] << 8 | data[index + 1];
                index += 2;
                if(num_nalus != 1)
                {
                    std::cout << "hevc only support 1 sps/pps/vps" << std::endl;
                    return true;
                }
                int16_t nal_unit_length = data[index] << 8 | data[index + 1];
                index += 2;
                //vps
                if(32 == unit_type){
                   std::cout << "vps" << std::endl;
                }else if(33 == unit_type){
                    std::cout << "sps" << std::endl;
                }else if(34 == unit_type) {
                    std::cout << "pps" << std::endl;
                }else{
                    std::cout << "not support unit type" << std::endl;
                    continue;
                }

                if(_out_video_file) {
                    fwrite(H264_START_CODE, 1, H264_START_CODE_LENGTH, _out_video_file);
                    fwrite(data + index, 1, nal_unit_length, _out_video_file);
                }
                index += nal_unit_length;
            }

        } else {
            int index = 5;
            index++;//configurationVersion = 1
            index++; //AVCProfileIndication
            index++; //profile compatibility
            index++; //avclevelindication
            index++; //reserved lenthSizeMinusOne

            int spsSize = data[index++] & 0x1f;//sps size
            std::cout << "sspsize:" << spsSize << std::endl;
            for (int i = 0; i < spsSize; i++) {
                int spsLength = data[index] << 8 | data[index + 1];
                std::cout << "spsLength:" << spsLength << std::endl;
                index += 2;
                if (_out_video_file) {
                    fwrite(H264_START_CODE, 1, H264_START_CODE_LENGTH, _out_video_file);
                    fwrite(data + index, 1, spsLength, _out_video_file);
                    index += spsLength;
                }
            }

            int ppsSize = data[index++] & 0xff;//pps size
            std::cout << "ppsSize:" << ppsSize << std::endl;
            for (int i = 0; i < ppsSize; i++) {
                int ppsLength = data[index] << 8 | data[index + 1];
                std::cout << "ppsLength:" << ppsLength << std::endl;
                index += 2;
                if (_out_video_file) {
                    fwrite(H264_START_CODE, 1, H264_START_CODE_LENGTH, _out_video_file);
                    fwrite(data + index, 1, ppsLength, _out_video_file);
                    index += ppsLength;
                }
            }

        }

    } else if (packet_type == 1) {
        //NALU len + NALU
        if (_out_video_file) {
            fwrite(H264_START_CODE, 1, H264_START_CODE_LENGTH, _out_video_file);
            fwrite(data + 5 + 4, 1, len - 5 - 4, _out_video_file);
        }
    } else if (packet_type == 2) {

    } else {
        std::cout << "no support packet type, id:" << packet_type << std::endl;
    }

    return true;
}

bool FlvParser::read_audio_tag(TagHeader header, uint8_t *data, int len)
{
    uint8_t audio_codec_type = data[0] >> 4;
    uint8_t sample_type = data[0] & 0x0C;
    uint32_t audio_sample = 0;
    if (sample_type == 0) {
        audio_sample = 5500;
    } else if (sample_type == 1) {
        audio_sample = 11100;
    } else if (sample_type == 2) {
        audio_sample = 22200;
    } else {
        audio_sample = 44000;
    }

    uint32_t sample_byte = ((data[0] & 0x02) == 0 ? 8 : 16);
    uint32_t channels = ((data[0] & 0x01) == 0 ? 1 : 2);
    //aac
    if (audio_codec_type == 10) {
        std::cout << "audio_codec:" << (int) audio_codec_type << ", sample:" << audio_sample
                  << ", sample_type:" << sample_byte << ", channels:" << channels << std::endl;
        //sequence header
        if (data[1] == 0) {
            //AudioSpecificConfig 2字节定义如下：
            //AAC Profile 5bits | 采样率 4bits | 声道数 4bits | 其他 3bits |
            _aac_spec_config.aac_profile = data[2] && 0xf8 >> 3  - 1;
            /*
             * 采样率 4bits
            Value samplingFrequencyIndex
            0x00   96000
            0x01   88200
            0x02   64000
            0x03   48000
            0x04   44100
            0x05   32000
            0x06   24000
            0x07   22050
            0x08   16000
            0x09   12000
            0x0A   11025
            0x0B     8000
            0x0C   reserved
            0x0D   reserved
            0x0E   reserved
            0x0F   escape value
             */
            _aac_spec_config.aac_samples = ((data[2] & 0x07) << 1) | (data[3] & 0x80);
            _aac_spec_config.aac_channels = data[3] >> 3 && 0x0f;
        } else if (data[1] == 1) {

            char aac_adts_header[7] = {0x00};

            aac_adts_header[0] = 0xFF;
            aac_adts_header[1] = 0xF9;

            aac_adts_header[2] = (((_aac_spec_config.aac_profile - 1) << 6) +
                                  (_aac_spec_config.aac_samples << 2) + (_aac_spec_config.aac_channels >> 2));

            int aac_frame_len = header.data_size - 2 + 7;
            aac_adts_header[3] = (((_aac_spec_config.aac_channels & 3) << 6) + (aac_frame_len >> 11));
            aac_adts_header[4] = ((aac_frame_len & 0x7FF) >> 3);
            aac_adts_header[5] = (((aac_frame_len & 7) << 5) + 0x1F);
            aac_adts_header[6] = 0xFC;

            //raw
            if (_out_audio_file) {
                fwrite(aac_adts_header, 1, 7, _out_audio_file);
                fwrite(data + 2, 1, len - 2, _out_audio_file);
            }
        }
    } else {
        std::cout << "no support audio codec,id:" << (uint32_t) audio_codec_type << std::endl;
    }
    return false;
}