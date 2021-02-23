//
// Created by dong on 2021/2/20.
//

#ifndef FFMPEG_SIMPLEDECODE_H
#define FFMPEG_SIMPLEDECODE_H

#include "../prech.h"




int ffmpeg_decode(int argc, char* argv[])
{
    AVFormatContext	*pFormatCtx;
    int				i, videoindex;
    AVCodecContext	*pCodecCtx;
    AVCodec			*pCodec;
    AVFrame	*pFrame,*pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    //输入文件路径
    char filepath[]=FILEPATH;

    int frame_cnt;

//    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        printf("Couldn't find stream information.\n");
        return -1;
    }
    videoindex=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            videoindex=i;
            break;
        }
    if(videoindex==-1){
        printf("Didn't find a video stream.\n");
        return -1;
    }

    pCodecCtx=pFormatCtx->streams[videoindex]->codec;
    AVCodecParameters *pCodecPar = pFormatCtx->streams[videoindex]->codecpar;
    pCodec=avcodec_find_decoder(pCodecPar->codec_id);


    if(pCodec==NULL){
        printf("Codec not found.\n");
        return -1;
    }
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
        printf("Could not open codec.\n");
        return -1;
    }
    /*
     * 在此处添加输出视频信息的代码
     * 取自于pFormatCtx，使用fprintf()
     */

    /**************************/
    fprintf(stdout,"heght = %d \n",pCodecPar->height);
    fprintf(stdout,"width = %d",pCodecPar->width);
    fprintf(stdout,"fr = %d",pCodecCtx->framerate.num/pCodecCtx->framerate.den);

    /**************************/

    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,
                                                       pCodecPar->width, pCodecPar->height));
    /*填充AVFrame 的数据缓冲*/
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P,
                   pCodecPar->width, pCodecPar->height);

    packet=(AVPacket *)av_malloc(sizeof(AVPacket));
    //Output Info-----------------------------
    printf("\n--------------- File Information ----------------\n");
    av_dump_format(pFormatCtx,0,filepath,0);
    printf("-------------------------------------------------\n");
    /*Allocate and return an SwsContext. You need it to perform
     *scaling/conversion operations using sws_scale().
     */
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height,
                                     AV_PIX_FMT_YUV420P, SWS_BICUBIC,
                                     NULL, NULL, NULL);

    frame_cnt=0;
    FILE *fp_h264 = fopen("simple.h264", "wb+");
    FILE *fp_yuv  = fopen("simple.yuv","wb+");

    while(av_read_frame(pFormatCtx, packet)>=0){
        if(packet->stream_index==videoindex){
            /*
             * 在此处添加输出H264码流的代码
             * 取自于packet，使用fwrite()
             */

            /******************************/

            fwrite(packet->data, packet->size, 1, fp_h264);


            /******************************/

            /*将单帧数据解码 pack->frame*/
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if(ret < 0){
                printf("Decode Error.\n");
                return -1;
            }
            if(got_picture){
                /*
                 * 去除一些图像边缘不许的数据
                 * */
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameYUV->data, pFrameYUV->linesize);
                printf("Decoded frame index: %d\n",frame_cnt);

                /*
                 * 在此处添加输出YUV的代码
                 * 取自于pFrameYUV，使用fwrite()
                 */
                fwrite(pFrame->data[0],pCodecCtx->width*pCodecCtx->height,1,fp_yuv);
                fwrite(pFrame->data[1],pCodecCtx->width*pCodecCtx->height/4,1,fp_yuv);
                fwrite(pFrame->data[2],pCodecCtx->width*pCodecCtx->height/4,1,fp_yuv);
                frame_cnt++;

            }
        }
        av_free_packet(packet);
    }

    fclose(fp_h264);
    fclose(fp_yuv);
    sws_freeContext(img_convert_ctx);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}




#endif //FFMPEG_SIMPLEDECODE_H
