//
// Created by dong on 2021/2/8.
//

#include "MedieInfo.h"
#include "../prech.h"



int MedieInfo::run()
{
    AVFormatContext *fmtCtx1 = NULL;
    int ret = -1;

    //open file ,must be close
    ret = avformat_open_input(&fmtCtx1,fileName.c_str(),NULL,NULL);

    if( 0 != ret)

    {
        std::cerr<<"Could not open file\n"<<std::endl;
        return ret;
    }

    ret = avformat_find_stream_info(fmtCtx1,NULL);


    AVCodec *codec = NULL;
    ret = av_find_best_stream(fmtCtx1,AVMEDIA_TYPE_VIDEO,
                        -1,-1,&codec,0);

    AVStream  *videoStream = NULL;
    AVStream  *audioStream = NULL;

    videoIndex = -1, audioIndex = -1;

    for(int i = 0 ; i < fmtCtx1->nb_streams;i++)
    {
        if(fmtCtx1->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoIndex = i;
            break;
        }
    }
    for(int i = 0 ; i < fmtCtx1->nb_streams;i++)
    {
        if(fmtCtx1->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioIndex = i;
            break;
        }
    }

    AVCodecParameters *codePar = fmtCtx1->streams[videoIndex]->codecpar;

    /**************可以获取一些媒体信息了*************/
    av_dump_format(fmtCtx1,0,fileName.c_str(),0);



    openCodeContext(videoIndex,NULL,fmtCtx1,AVMEDIA_TYPE_VIDEO);




    avformat_free_context(fmtCtx1);
    if( 0 != ret)
    {
        std::cerr<<"avformat_alloc_output_context2 failed = "<<ret<< std::endl;
    }

    return ret;
}

int MedieInfo::openCodeContext(int &streamIdx,AVCodecContext** pAvCodecContext,
                    AVFormatContext * pAvFormatContext,enum AVMediaType type)
{
    int ret = -1;
    if(!pAvFormatContext)
        return ret;
    ret = av_find_best_stream(pAvFormatContext,type,
                              -1,-1,NULL,0);
    if(ret < 0 )
    {
        std::cerr<<"error: not find base stream index\n"<<std::endl;
        return ret;
    }

    int streamIndex = ret;
    AVStream *st = pAvFormatContext->streams[streamIndex];
    AVCodecParameters *pCodecpar = st->codecpar;

    AVCodec* decodec = avcodec_find_decoder(st->codecpar->codec_id);
    if(!decodec)
    {
        std::cerr<<"error : not find decodec\n"<<std::endl;
        return AVERROR(EINVAL);
    }

    /*叫高版本弃用了st->codec 属性*/
    /*
     * 应该是因为原来的codecContext 太过复杂
     * 开发者试图将部分逻辑抽象出来
     */
//    AVCodecContext *pDecodecContext = st->codec;

    AVCodecContext* pCodecContext = avcodec_alloc_context3(decodec);
    ret = avcodec_parameters_to_context(pCodecContext, pCodecpar);
    if( ret < 0)
    {
        std::cerr<<"error : parameters to context = "<<ret<<std::endl;
        return AVERROR(ret);
    }

    //
   ret = avcodec_open2(pCodecContext,decodec, NULL);
    if(0 != ret)
    {
        std::cerr<<"error : avcodec open failed = "<<ret<<std::endl;
        return AVERROR(ret);
    }

    AVFrame  *frame = av_frame_alloc();
    AVFrame *frameYur = av_frame_alloc();

    uint8_t * out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,
                                                       pCodecpar->width, pCodecpar->height));
    /*填充AVFrame 的数据缓冲*/
    avpicture_fill((AVPicture *)frameYur, out_buffer, AV_PIX_FMT_YUV420P,
                   pCodecpar->width, pCodecpar->height);


    AVPacket *packet = av_packet_alloc();

    /*Allocate and return an SwsContext. You need it to perform
     *scaling/conversion operations using sws_scale().
     */
    SwsContext* imageSwsContext = sws_getContext(pCodecpar->width,pCodecpar->height,pCodecContext->pix_fmt,
                   pCodecpar->width,pCodecpar->height,AV_PIX_FMT_YUV420P,
                   SWS_BICUBIC,NULL,NULL,NULL);

    int goPicture = -1;
    FILE *fp_yuv = fopen("simple.yuv","wb+");
    //从formatContext 数据流中回去单帧数据
    int frame_cnt = 0;
    while (av_read_frame(pAvFormatContext,packet)>= 0)
    {
        if(packet->stream_index == videoIndex)
        {
            //packet 压缩过的单帧
//            avcodec_decode_video2(pCodecContext,frame,&goPicture,packet);
            ret = avcodec_send_packet(pCodecContext,packet);
            if(0 != ret)
            {
                return ret;
            }
            //注意 receive_frame 一些情况下， 未必可以一次性获取所以的数据，可以通过返回值 多次获取

            if(avcodec_receive_frame(pCodecContext,frame) == 0)
            {
                //解码后的数据
                // dong something
                sws_scale(imageSwsContext, (const uint8_t* const*)frame->data,
                          frame->linesize, 0, pCodecContext->height,
                          frameYur->data, frameYur->linesize);
                printf("Decoded frame index: %d\n",frame_cnt);

                /*
                 * 在此处添加输出YUV的代码
                 * 取自于pFrameYUV，使用fwrite()
                 */
                fwrite(frame->data[0],pCodecContext->width*pCodecContext->height,1,fp_yuv);
                fwrite(frame->data[1],pCodecContext->width*pCodecContext->height/4,1,fp_yuv);
                fwrite(frame->data[2],pCodecContext->width*pCodecContext->height/4,1,fp_yuv);
                frame_cnt++;

            }

        }else if (packet->stream_index == audioIndex)
        {

        }
    }
    fclose(fp_yuv);

}



