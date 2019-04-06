/*************************************************************************
	> File Name: FlvParser.h
	> Author: amoscykl
	> Mail: amoscykl@163.com 
	> Created Time: 2019年03月23日 星期六 22时26分48秒
 ************************************************************************/

#include<iostream>

using namespace std;

#define FLV_HEADER_DEFAULT_SIZE 9
#define FLV_TAG_HEADER_DEFAULT_SIZE 11

enum
{
    FLV_TAG_TYPE_VIDEO = 0x09,
    FLV_TAG_TYPE_AUDIO = 0x08,
    FLV_TAG_TYPE_METADATA = 0x12,
};

enum
{
    FLV_VIDEO_CODEC_H263 = 2,
    FLV_VIDEO_CODEC_SCREEN = 3,
    FLV_VIDEO_CODEC_VP6 = 4,
    FLV_VIDEO_CODEC_VP6A = 5,
    FLV_VIDEO_CODEC_SCREEN2 = 6,
    FLV_VIDEO_CODEC_H264 = 7,
    FLV_VIDEO_CODEC_REALH263 = 8,
    FLV_VIDEO_CODEC_MPEG4 = 9,
    FLV_VIDEO_CODEC_HEVE = 12,

};

#define FLV_VIDEO_FRAMETYPE_OFFSET 4

enum
{
    FLV_FRAME_KEY                   = 1 << FLV_VIDEO_FRAMETYPE_OFFSET,
    FLV_FRAME_INTER                 = 2 << FLV_VIDEO_FRAMETYPE_OFFSET,
    FLV_FRAME_DISP_INTER            = 3 << FLV_VIDEO_FRAMETYPE_OFFSET,
    FLV_FRAME_GENERATED_KEY         = 4 << FLV_VIDEO_FRAMETYPE_OFFSET,
    FLV_FRAME_VIDEO_INFO_CMD        = 5 << FLV_VIDEO_FRAMETYPE_OFFSET,
};

#define FLV_AUDIO_CODECID_OFFSET 4
enum
{
    FLV_AUDIO_CODEC_PCM                  = 0,
    FLV_AUDIO_CODEC_ADPCM                = 1 << FLV_AUDIO_CODECID_OFFSET,
    FLV_AUDIO_CODEC_MP3                  = 2 << FLV_AUDIO_CODECID_OFFSET,
    FLV_AUDIO_CODEC_PCM_LE               = 3 << FLV_AUDIO_CODECID_OFFSET,
    FLV_AUDIO_CODEC_NELLYMOSER_16KZ_MONO = 4 << FLV_AUDIO_CODECID_OFFSET,
    FLV_AUDIO_CODEC_NELLYMOSER_8KZ_MONO  = 5 << FLV_AUDIO_CODECID_OFFSET,
    FLV_AUDIO_CODEC_NELLYMOSER           = 6 << FLV_AUDIO_CODECID_OFFSET,
    FLV_AUDIO_CODEC_PCM_ALAW             = 7 << FLV_AUDIO_CODECID_OFFSET,
    FLV_AUDIO_CODEC_PCM_MULAW            = 8 << FLV_AUDIO_CODECID_OFFSET,
    FLV_AUDIO_CODEC_AAC                  = 10 << FLV_AUDIO_CODECID_OFFSET,
    FLV_AUDIO_CODEC_SPEEX                = 11 << FLV_AUDIO_CODECID_OFFSET,


};

struct FlvHeader
{
    int version;
    bool is_video;
    bool is_audio;
    int header_size;

    FlvHeader()
    {
        version = 0;
        is_video = false;
        is_audio = false;
        header_size = FLV_HEADER_DEFAULT_SIZE;
    }

};

struct TagHeader
{
    int type;
    int data_size;
    int stamp;
    int string_id;
};

struct AudioSpecificConfig
{
    int aac_profile;
    int aac_samples;
    int aac_channels;
};
class FlvParser
{
private:
    FlvHeader _header;
    std::string _file_name;
    bool _is_flv;
    FILE* _file;

    std::string _out_video_file_name;
    std::string _out_audio_file_name;
    FILE* _out_video_file;
    FILE* _out_audio_file;
    AudioSpecificConfig _aac_spec_config;
private:
    bool read_header();
    bool read_video_tag(TagHeader header, uint8_t* data, int len);
    bool read_audio_tag(TagHeader header, uint8_t* data, int len);
public:
    explicit FlvParser(const char *file_name);
    FlvParser(const char* file_name, const char* out_video_file_name, const char* out_audio_file_name);
    ~FlvParser();

    bool read_next_tag();
};

