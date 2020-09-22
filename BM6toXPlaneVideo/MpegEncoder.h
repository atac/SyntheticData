#pragma once

// https://ffmpeg.org/ffmpeg-codecs.html

// Look at AVFormatContext for mpegts
// https://ffmpeg.org/ffmpeg-all.html#mpegts-1

// http://dranger.com/ffmpeg/tutorial05.html

// http://codefromabove.com/2014/10/ffmpeg-convert-rgba-to-yuv/

extern "C" {
#include "libavutil/common.h"
#include <libavutil/opt.h>
#include "libavutil/dict.h"
#include "libavutil/log.h"
#include "libavformat/avio.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "libavformat/version.h"
}

//#pragma comment(lib, "avcodec.lib")

#define RGB_FORMAT

class ClMpegEncoder
    {
    // Constructor / Destructor    
public:
    ClMpegEncoder();
    ~ClMpegEncoder();

    // Data
private:
    typedef int (*Ffmpeg_Write_t)(void *opaque, uint8_t *buf, int buf_size);

    unsigned char     * pIOBuffer          = nullptr;
    AVIOContext       * pAVIOContext       = nullptr;
    AVFormatContext   * pFormatContext     = nullptr;
    AVCodecContext    * pVideoCodecContext = nullptr;
    AVCodec           * pVideoCodec        = nullptr;
    AVStream          * pVideoStream       = nullptr;
    AVPacket          * pAVPacket          = nullptr;
    struct SwsContext * psuScalingContext  = nullptr;
    unsigned int        uSrcHeight;
    unsigned int        uSrcWidth;

public:
    uint8_t           * pSrcData[4];    // Array needed for YUV formats
    int                 aiSrcLineSizes[4];
    AVFrame           * pAVFrame           = nullptr;

public:  
    // Methods
//    bool Open(const char * szFilename, unsigned int uHeight, unsigned int uWidth, int iFrameRate, unsigned long lBitRate);
    bool Open(Ffmpeg_Write_t pWriteFunc, void * pvWriteFuncData, unsigned int uHeight, unsigned int uWidth, int iFrameRate);
    void Close();
    bool Encode();
    void Cleanup();

    };

