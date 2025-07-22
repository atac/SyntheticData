
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "MpegEncoder.h"

#if defined(VIDEO_FMT0_MPEG)

#endif

#if defined(VIDEO_FMT0_H264)

#endif

// Video as per MISP Recommended Practice 9720d System Level L4H (IRIG Video Format 0)
#define OUTPUT_WIDTH    1280
#define OUTPUT_HEIGHT   720
#define BIT_RATE        10000000
//#define OUTPUT_WIDTH    uSrcWidth
//#define OUTPUT_HEIGHT   uSrcHeight

//#define ENCODER_H264
//#define ENCODER_MPEG2
    
//#define PIX_FORMAT      AV_PIX_FMT_RGB24
//#define PIX_FORMAT      AV_PIX_FMT_BGRA
//#define PIX_FORMAT      AV_PIX_FMT_BGR0 

#define PIX_FORMAT_SCREEN   AV_PIX_FMT_BGR0

//#define PIX_FORMAT_ENCODE   AV_PIX_FMT_BGR0
#define PIX_FORMAT_ENCODE   AV_PIX_FMT_YUV420P

// https://www.videolan.org/developers/x264.html
// https://www.codeproject.com/tips/489450/creating-custom-ffmpeg-io-context
// https://trac.ffmpeg.org/wiki/Encode/H.264

#define CUSTOM_AVIO

// ============================================================================

ClMpegEncoder::ClMpegEncoder()
    {
    }


// ----------------------------------------------------------------------------

ClMpegEncoder::~ClMpegEncoder()
    {
//    Cleanup();
    }

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

//bool ClMpegEncoder::Open(const char * szFilename, unsigned int uHeight, unsigned int uWidth, int iFrameRate, unsigned long lBitRate)
bool ClMpegEncoder::Open(Ffmpeg_Write_t pWriteFunc, void * pvWriteFuncData, unsigned int uHeight, unsigned int uWidth, int iFrameRate)
    {
#if 1
    const char      * szCodecName;
    const char      * szOutputFormatName;

    int               ret;

    uSrcHeight = uHeight;
    uSrcWidth  = uWidth;

//#define FRAME_RATE  25

// OK
// mpeg1video mpegts     25
// mpeg1video mpeg2video 25
// libx264    mpegts     25
// libx265    mpegts     25
// mpeg4      mpegts     25
// mpeg4      mpegts     20
// mpeg4      mpegts     10

// Not OK
// mpeg2video mpegts     25
// mpeg2video mpeg2video 25
// libx264    mpeg2video 25
// msmpeg4    mpegts     25
// msmpeg4    mpeg2video 25
// mpeg1video mpegts     10
// mpeg2video mpegts     10
// libx264    mpegts     10

// AV_CODEC_ID_MPEG2TS

//    szCodecName        = "mpeg2video";
//    szCodecName        = "mpeg1video";
#ifndef RGB_FORMAT
    szCodecName        = "libx264";
#else
//    szCodecName        = "libx264rgb";
    szCodecName        = "mpeg2video";
//    szCodecName        = "mpeg2ts";
#endif
//    szCodecName        = "libx265";
//    szCodecName        = "mpeg4";
//    szCodecName        = "msmpeg4";
//    szCodecName        = "msvideo1";
//    szCodecName        = "wmv1";
//    szCodecName        = "wmv2";

    // From libavformat/muxer_list.c
    szOutputFormatName = "";
//    szOutputFormatName = "mpeg";          // MPEG-1 Systems / MPEG program stream
//    szOutputFormatName = "mpeg1video";    // raw MPEG-1 video
//    szOutputFormatName = "mpeg2video';    // raw MPEG-2 video
//    szOutputFormatName = "mpegts";        // MPEG-TS (MPEG-2 Transport Stream)
//    szOutputFormatName = "mjpeg";         // Raw MJPEG video
//    szOutputFormatName = "mpjpeg";        // MIME multipart JPEG

    // AVIOContext
    // -----------

#ifndef CUSTOM_AVIO
    // Open the output file to write to it.
    if ((ret = avio_open(&pAVIOContext, szFilename, AVIO_FLAG_WRITE)) < 0) 
        {
        char szErrStr[AV_ERROR_MAX_STRING_SIZE] = {0};
        fprintf(stderr, "Could not open output file '%s' (error '%s')\n", szFilename, 
                av_make_error_string(szErrStr, AV_ERROR_MAX_STRING_SIZE, ret));
        exit(1);
        }
#else
    // Create internal Buffer for FFmpeg:
    const int iBufSize = 10000 * 188;
    pIOBuffer = (unsigned char *)malloc(iBufSize);
 
    // Allocate the AVIOContext:
    // The fourth parameter (pStream) is a user parameter which will be passed to our callback functions
    pAVIOContext = avio_alloc_context(pIOBuffer, iBufSize,  // internal Buffer and its size
                                      1,                    // bWriteable (1=true,0=false) 
                                      pvWriteFuncData,      // user data ; will be passed to our callback functions
                                      NULL,
                                      pWriteFunc, 
                                      NULL); 

#endif

    // SwsContext
    // ----------

    psuScalingContext = sws_getContext(uWidth, uHeight, PIX_FORMAT_SCREEN,
                             OUTPUT_WIDTH, OUTPUT_HEIGHT, PIX_FORMAT_ENCODE,
                             SWS_BILINEAR, NULL, NULL, NULL);
    if (!psuScalingContext) 
        {
        fprintf(stderr,
                "Impossible to create scale context for the conversion ");
                //"fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
                //av_get_pix_fmt_name(SRC_FORMAT), SRC_WIDTH, SRC_HEIGHT,
                //av_get_pix_fmt_name(DST_FORMAT), DST_WIDTH, DST_HEIGHT);
        ret = AVERROR(EINVAL);
        return false;
        }

    ret = av_image_alloc(pSrcData, aiSrcLineSizes, uWidth, uHeight, PIX_FORMAT_SCREEN, 16);
    if (ret < 0) 
        {
        fprintf(stderr, "Could not allocate source image\n");
        return false;
        }

    // AVCodec
    // -------

    // Setup the video encoder
    pVideoCodec = avcodec_find_encoder_by_name(szCodecName);
    if (!pVideoCodec) 
        {
        fprintf(stderr, "Codec '%s' not found\n", szCodecName);
        return false;
        }

    // AVCodecContext
    // --------------

    // Setup the video encoder context
    pVideoCodecContext = avcodec_alloc_context3(pVideoCodec);

    // Put sample parameters
    pVideoCodecContext->bit_rate = BIT_RATE;

    // Resolution must be a multiple of two
    pVideoCodecContext->width  = OUTPUT_WIDTH;
    pVideoCodecContext->height = OUTPUT_HEIGHT;

    // Frames per second
    pVideoCodecContext->framerate = AVRational{iFrameRate, 1};
    pVideoCodecContext->time_base = AVRational{1, iFrameRate};

    /* Emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    pVideoCodecContext->gop_size     = 25;
//    pVideoCodecContext->max_b_frames = 2;
#ifndef RGB_FORMAT
    pVideoCodecContext->pix_fmt      = AV_PIX_FMT_YUV420P;
#else
    pVideoCodecContext->pix_fmt      = PIX_FORMAT_ENCODE;
#endif

//    pVideoCodecContext->

    // libx264
    // AV_PIX_FMT_YUV420P   planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    // AV_PIX_FMT_YUVJ420P
    // AV_PIX_FMT_YUV422P
    // AV_PIX_FMT_YUVJ422P
    // AV_PIX_FMT_YUV444P
    // AV_PIX_FMT_YUVJ444P
    // AV_PIX_FMT_NV12
    // AV_PIX_FMT_NV16

    // libx264rgb
    // AV_PIX_FMT_BGR0      packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined
    // AV_PIX_FMT_BGR24     packed RGB 8:8:8, 24bpp, BGRBGR...
    // AV_PIX_FMT_RGB24     packed RGB 8:8:8, 24bpp, RGBRGB...

// Default profile High 4:4:4 Predictive, level 1.3, 4:4:4, 8-bit
//    pVideoCodecContext->profile = FF_PROFILE_H264_BASELINE;
//    pVideoCodecContext->profile = FF_PROFILE_H264_MAIN;
//    pVideoCodecContext->profile = FF_PROFILE_H264_HIGH_444_INTRA;

    /* just for testing, we also add B-frames */
    //if (pVideoCodecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO) 
    //    pVideoCodecContext->max_b_frames = 2;
            
    /* Needed to avoid using macroblocks in which some coeffs overflow.
     * This does not happen with normal video, it just happens here as
     * the motion of the chroma plane does not match the luma plane. */
    //if (pVideoCodecContext->codec_id == AV_CODEC_ID_MPEG1VIDEO) 
    //    pVideoCodecContext->mb_decision = 2;

    if (pVideoCodec->id == AV_CODEC_ID_H264)
        {
        /* include/x264.h:static const char * const x264_preset_names[] = 
            { "ultrafast", "superfast", "veryfast", "faster", "fast", 
              "medium", "slow", "slower", "veryslow", "placebo", 0 };
        */

//        av_opt_set(pVideoCodecContext->priv_data, "preset", "slow", 0);
        av_opt_set(pVideoCodecContext->priv_data, "preset", "fast", 0);
//        av_opt_set(pVideoCodecContext->priv_data, "preset", "ultrafast", 0);

        // Set the quality for constant quality mode. 
//        av_opt_set(pVideoCodecContext->priv_data, "crf", "10", 0);
        av_opt_set(pVideoCodecContext->priv_data, "crf", "23", 0);
//        av_opt_set(pVideoCodecContext->priv_data, "crf", "32", 0);
        }

    if (pVideoCodec->id == AV_CODEC_ID_MPEG2VIDEO)
        {
//        pVideoCodecContext->max_b_frames = 2;
//pVideoCodecContext->bit_rate = 200000;
//pVideoCodecContext->time_base.den = 1;
//pVideoCodecContext->time_base.num = 90000;
        // pVideoCodecContext->gop_size = 8;
        }

    // AVFormatContext
    // ---------------

    // Get the output container format context
    // Create a new AVFormatContext (pFormatContext) by calling avformat_alloc_context().
    // If passed oformat (m_pOutputFormat) is NULL it creates one and attaches
    // it to the newly created AVFormatContext (pFormatContext). But it creates oformat passed
    // "guesses" from the filename or passed format name.

//  avformat_alloc_output_context2(&pFormatContext, NULL, NULL, filename);
    if (strlen(szOutputFormatName) != 0)
        avformat_alloc_output_context2(&pFormatContext, NULL, szOutputFormatName, NULL);
    else
#ifndef CUSTOM_AVIO
        avformat_alloc_output_context2(&pFormatContext, NULL, NULL, szFilename);
#else
        avformat_alloc_output_context2(&pFormatContext, NULL, "mpegts", NULL);
#endif
    if (pFormatContext == NULL)
        return false;

    // Associate the output file (pointer) with the container format context.
    pFormatContext->pb             = pAVIOContext;
    pFormatContext->video_codec    = pVideoCodec;
    pFormatContext->video_codec_id = pVideoCodec->id;
#ifdef CUSTOM_AVIO
    pFormatContext->flags          = AVFMT_FLAG_CUSTOM_IO;
#endif

    // AVStream
    // --------

    // Add a video stream to the output
    pVideoStream = avformat_new_stream(pFormatContext, pVideoCodec);
    if (!pVideoStream) 
        {
        fprintf(stderr, "Could not allocate stream\n");
        return false;
        }
    pVideoStream->id                   = pFormatContext->nb_streams - 1;
    pVideoStream->index                = pFormatContext->streams[0]->index;
    pVideoStream->time_base            = pVideoCodecContext->time_base;
    pVideoStream->codecpar->codec_id   = pVideoCodec->id;
    pVideoStream->codecpar->codec_type = pVideoCodec->type;
    pVideoStream->codecpar->width      = pVideoCodecContext->width;
    pVideoStream->codecpar->height     = pVideoCodecContext->height;

    // AVPacket
    // --------

    // Get a video packet
    pAVPacket = av_packet_alloc();
    if (!pAVPacket)
        return false;

    // AVFrame
    // -------

    // Get a frame to hold the raw video
    pAVFrame = av_frame_alloc();
    if (!pAVFrame) 
        {
        fprintf(stderr, "Could not allocate video frame\n");
        return false;
        }
    pAVFrame->format = pVideoCodecContext->pix_fmt;
    pAVFrame->width  = pVideoCodecContext->width;
    pAVFrame->height = pVideoCodecContext->height;

    // Allocate a frame buffer to hold the raw video data
#ifndef RGB_FORMAT
    ret = av_frame_get_buffer(pAVFrame, 32);
#else
    ret = av_frame_get_buffer(pAVFrame, 0);
#endif
    if (ret < 0) 
        {
        fprintf(stderr, "Could not allocate the video frame data\n");
        return false;
        }

    // Make sure everything is open and then get started

    // Open the codec
    ret = avcodec_open2(pVideoCodecContext, pVideoCodec, NULL);
    if (ret < 0) 
        {
        char szErrStr[AV_ERROR_MAX_STRING_SIZE] = {0};
        fprintf(stderr, "Could not open codec: %s\n", 
                av_make_error_string(szErrStr, AV_ERROR_MAX_STRING_SIZE, ret));
        return false;
        }

	//Write File Header
	avformat_write_header(pFormatContext, NULL);

#endif
    return true;
    }


// ----------------------------------------------------------------------------

void ClMpegEncoder::Close()
    {
    AVFrame         * pAVFrameTemp;

    // Save the AVFrame
    pAVFrameTemp = pAVFrame;
    pAVFrame     = NULL;

    // Flush the encoder and close the file
    Encode();
    av_write_trailer(pFormatContext);
#ifndef CUSTOM_AVIO
    avio_closep(&(pFormatContext->pb));
#else
#endif

    // Restore the AVFrame
    pAVFrame = pAVFrameTemp;

    Cleanup();

    return;
    }


// ----------------------------------------------------------------------------

void ClMpegEncoder::Cleanup()
    {
    avformat_free_context(pFormatContext);
    avcodec_free_context(&pVideoCodecContext);
    av_frame_free(&pAVFrame);
    av_packet_free(&pAVPacket);
#ifdef CUSTOM_AVIO
    av_free(pAVIOContext);
    free(pIOBuffer);
    pIOBuffer = nullptr;
#endif
    }

// ----------------------------------------------------------------------------

bool ClMpegEncoder::Encode()
    {
    int ret;

    // Convert to destination size and format
    sws_scale(psuScalingContext, pSrcData, aiSrcLineSizes, 0, uSrcHeight, pAVFrame->data, pAVFrame->linesize);

    /* send the frame to the encoder */
    //if (pAVFrame)
    //    printf("Send frame %3lld\n", pAVFrame->pts);

    ret = avcodec_send_frame(pVideoCodecContext, pAVFrame);
    if (ret != 0) 
        {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
        }

    while (ret >= 0) 
        {
        ret = avcodec_receive_packet(pVideoCodecContext, pAVPacket);
        if (ret == AVERROR(EAGAIN))
            return false;
        else if (ret == AVERROR_EOF)
            return false;
        else if (ret < 0) 
            {
            fprintf(stderr, "Error during encoding\n");
            return false;
            }

        // rescale output packet timestamp values from codec to stream timebase
        pAVPacket->stream_index = 0; // Hard coded for now
        av_packet_rescale_ts(pAVPacket, pVideoCodecContext->time_base, pFormatContext->streams[0]->time_base);

//      printf("Write packet %3lld (size=%5d)\n", pAVPacket->pts, pAVPacket->size);
//      av_interleaved_write_frame(fmt_ctx, pAVPacket);
        av_write_frame(pFormatContext, pAVPacket);

        av_packet_unref(pAVPacket);
        }

    return true;
    }


// ----------------------------------------------------------------------------

