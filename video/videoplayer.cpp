﻿/**
 * 李震
 * 我的码云：https://git.oschina.net/git-lizhen
 * 我的CSDN博客：http://blog.csdn.net/weixin_38215395
 * 联系：QQ1039953685
 */

#include "videoplayer.h"
extern "C"
{

    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"

}

#include <stdio.h>
#include<iostream>
using namespace std;
VideoPlayer::VideoPlayer()
{
    rtspaddr="rtsp://admin:123456@192.168.123.104:554/stream1";
}

VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::setrtsp(QString addr){
    rtspaddr=addr;
}

void VideoPlayer::startPlay()
{
    ///调用 QThread 的start函数 将会自动执行下面的run函数 run函数是一个新的线程
    this->start();
}

void VideoPlayer::stopPlay(){
    mStopFlag= true;
}
void VideoPlayer::run()
{
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    const AVCodec *pCodec;
    AVFrame *pFrame, *pFrameRGB;
    AVPacket *packet;
    uint8_t *out_buffer;

    static struct SwsContext *img_convert_ctx;

    int videoStream, i, numBytes;
    int ret, got_picture;

    avformat_network_init();

    //Allocate an AVFormatContext.
    pFormatCtx = avformat_alloc_context();


    AVDictionary *avdic=NULL;
    /*
    char option_key[]="rtsp_transport";
    char option_value[]="tcp";
    av_dict_set(&avdic,option_key,option_value,0);
    char option_key2[]="max_delay";
    char option_value2[]="100";
    av_dict_set(&avdic,option_key2,option_value2,0);*/

    av_dict_set(&avdic, "buffer_size", "1024000", 0); //设置最大缓存，1080可调到最大
    av_dict_set(&avdic, "rtsp_transport", "udp", 0); //以tcp的方式传送
    av_dict_set(&avdic, "stimeout", "5000000", 0); //设置超时断开链接时间，单位us
    av_dict_set(&avdic, "max_delay", "500000", 0); //设置最大时延
    av_dict_set(&avdic, "framerate", "5", 0);
    //av_dict_set(&avdic, "video_size","640x40",0);

    /*
    AVDictionary* options = NULL;
    av_dict_set(&options, "buffer_size", "1024000", 0); //设置最大缓存，1080可调到最大
    av_dict_set(&options, "rtsp_transport", "udp", 0); //以tcp的方式传送
    av_dict_set(&options, "stimeout", "5000000", 0); //设置超时断开链接时间，单位us
    av_dict_set(&options, "max_delay", "500000", 0); //设置最大时延
    av_dict_set(&options, "framerate", "20", 0);*/



    ///rtsp地址，可根据实际情况修改
    /// rtsp://127.0.0.1:8554/stream
    /// rtsp://admin:123456@192.168.123.104:554/stream1
    //char * tmp=(char*)rtspaddr.data();
    //char url[50];
    //strcpy(url, tmp);
    char url[] ="rtsp://admin:123456@192.168.123.104:554/stream1";

    if (avformat_open_input(&pFormatCtx, url, NULL, &avdic) != 0) {
        printf("can't open the file. \n");
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Could't find stream infomation.\n");
        return;
    }



    videoStream = -1;

    ///循环查找视频中包含的流信息，直到找到视频类型的流
    ///便将其记录下来 保存到videoStream变量中
    ///这里我们现在只处理视频流  音频流先不管他
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }

    ///如果videoStream为-1 说明没有找到视频流
    if (videoStream == -1) {
        printf("Didn't find a video stream.\n");
        return;
    }
    printf("nb_stream:%d videoStream:%d\n",pFormatCtx->nb_streams,videoStream);

    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);

    pCodecCtx = avcodec_alloc_context3(pCodec);

    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar);

    printf("pCodecCtx->frame_number:%d\n", pCodecCtx->frame_number);
    printf("pCodecCtx->time_base.num:%d\n", pCodecCtx->time_base.num);
    printf("pCodecCtx->time_base.den:%d\n", pCodecCtx->time_base.den);
    printf("pCodecCtx->bit_rate:%d\n", pCodecCtx->bit_rate);
    printf("pCodecCtx->framerate:%d\n", pCodecCtx->framerate);
    // pCodecCtx->bit_rate =0;   //初始化为0
    // pCodecCtx->time_base.num=1;  //下面两行：一秒钟25帧
    // pCodecCtx->time_base.den=10;
    // pCodecCtx->frame_number=1;  //每包一个视频帧


    if (pCodec == NULL) {
        printf("Codec not found.\n");
        return;
    }

    ///打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec.\n");
        return;
    }

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    ///这里我们改成了 将解码后的YUV数据转换成RGB32
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
            pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
            AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);

    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width,pCodecCtx->height,1);


    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

        av_image_fill_arrays(
                pFrameRGB->data,
                pFrameRGB->linesize,
                out_buffer,
                AV_PIX_FMT_RGBA,
                pCodecCtx->width,
                pCodecCtx->height,
                1
        );
    int y_size = pCodecCtx->width * pCodecCtx->height;

    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据
    mStopFlag = false;
    while (!mStopFlag)
    {
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            continue; //这里认为视频读取完了
        }

        if (packet->stream_index == videoStream) {
            ret = avcodec_send_packet(pCodecCtx,packet);
            if( 0 != ret){
                continue;
            }
            while (avcodec_receive_frame(pCodecCtx,pFrame) == 0){
                sws_scale(img_convert_ctx,
                        (uint8_t const * const *) pFrame->data,
                        pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                        pFrameRGB->linesize);

                //把这个RGB数据 用QImage加载
                QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGBA8888);

                //QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB888);
                QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
                emit sig_GetOneFrame(image);  //发送信号

                /*
                printf("pCodecCtx->width:%d\n", pCodecCtx->width);
                printf("pCodecCtx->height:%d\n", pCodecCtx->height);
                printf("pCodecCtx->frame_number:%d\n", pCodecCtx->frame_number);
                printf("pCodecCtx->time_base.num:%d\n", pCodecCtx->time_base.num);
                printf("pCodecCtx->time_base.den:%d\n", pCodecCtx->time_base.den);
                printf("pCodecCtx->bit_rate:%d\n", pCodecCtx->bit_rate);
                printf("pCodecCtx->framerate:%d\n", pCodecCtx->framerate);
                printf("pCodecCtx->frame_size:%d\n", pCodecCtx->frame_size);*/
            }
        }
        av_packet_unref(packet); //释放资源,否则内存会一直上升

        msleep(0.02); //停一停  不然放的太快了
    }
    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    //emit SigFinished();
}
