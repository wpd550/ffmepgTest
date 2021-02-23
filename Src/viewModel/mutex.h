#include <iostream>
#include "../prech.h"


#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <string>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

// a wrapper around a single output AVStream
typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *tmp_frame;

    float t, tincr, tincr2;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

static const int Stream_duration = 10;

#if DEBUGMRCO
static void addStream(OutputStream *ost,AVFormatContext *oc,AVCodec **codec, enum AVCodecID codec_id);

static void open_video(AVFormatContext* oc,AVCodec* codec,OutputStream *ost,AVDictionary *opt_arg);
static void open_audio(AVFormatContext* oc,AVCodec* codec,OutputStream *ost,AVDictionary *opt_arg);



static void findStreamInfo(AVFormatContext* oc);

static AVFrame* alloc_picture(enum AVPixelFormat pix_fmt,int width,int height);
static AVFrame* alloc_audio_frame(enum  AVSampleFormat sample_fmt,
                                  uint64_t channel_layout,
                                  int sample_rate,int nb_samples);

static  int write_video_frame(AVFormatContext *oc,OutputStream *ost);
static  int write_audio_frame(AVFormatContext *oc,OutputStream *ost);

static void close_stream(AVFormatContext *oc,OutputStream* ost);

static int startTest();

static void demo();
#endif

int run(){

#if DEBUGMRCO
    startTest();
#else
    startMutex(1,NULL);
#endif
    return 0;
}

#if DEBUGMRCO
char* fileName = "/Users/dong/Desktop/2020.mp4";

static void demo()
{
    printf("demo");
}
static int startTest() {
    int version = avformat_version();
    std::cout << "vsersion = "<<version<<std::endl;


    // 1注册函数，ffmpeg 4.0 版本之后不再需要了
    // av_register_all();

    OutputStream video_st = { 0},audio_st {0};

    // 初始化变量
    AVOutputFormat *fmt;
    AVFormatContext *oc;   //封装格式的上下问信息
    AVCodec *audio_codec,*video_codec;  //编码格式相关的结构

    int ret = -1;
    int have_video = 0, have_audio = 0;
    int encode_video = 0 ,encode_audio = 0;

    AVDictionary *opt = NULL;
    int  i;


    // 2 AVFormatContext 封装相关的操作,alloc output context
    ret = avformat_alloc_output_context2(&oc,NULL, NULL, fileName);
    if(!oc)
    {
        printf("cann't alloc mov format\n");
        exit(1);
    }
    fmt = oc->oformat;

//    findStreamInfo(oc);

    // 3申请AVStream 存放音频，视频，字幕
    //提起video 和 audio streams ，实用默认的fmt,AVStream 需要和AVCoder 结合实用
    if(fmt->audio_codec != AV_CODEC_ID_NONE)
    {
        addStream(&audio_st,oc,&audio_codec,fmt->audio_codec);
        have_audio = 1,encode_audio = 1;
    }
    if(fmt->video_codec != AV_CODEC_ID_NONE)
    {
        addStream(&video_st,oc,&video_codec,fmt->video_codec);
        have_video = 1,encode_video = 1;
    }

    //open audio and video codec and allocate the necessary encode buffers
    if(have_video)
    {
        open_video(oc,video_codec,&video_st,NULL);
    }
    if(have_audio)
    {
        open_audio(oc,audio_codec,&audio_st,NULL);
    }

    //打印输出信息
    av_dump_format(oc,0,fileName,1);

    /*open the output file,if needed*/
    if(!(fmt->flags & AVFMT_NOFILE)){
        ret = avio_open(&oc->pb,fileName,AVIO_FLAG_WRITE);
        if (0 != ret)
        {
            fprintf(stderr,"Could not open '%s': '%s'.\n",fileName,av_err2str(ret));
            return 1;
        }
    }

    //写入文件头
    ret =  avformat_write_header(oc,NULL);
    if(0 != ret)
    {
        fprintf(stderr,"Error occurred when opening output file: %s .\n",av_err2str(ret));
        return 1;
    }
    while ( encode_video || encode_audio){
        if (encode_video && (!encode_audio && av_compare_ts(video_st.next_pts,
                                                            video_st.enc->time_base,
                                                            audio_st.next_pts,
                                                            audio_st.enc->time_base) <= 0))
        {
            encode_video = !write_video_frame(oc,&video_st);
        }else{
            encode_audio = !write_audio_frame(oc,&audio_st);
        }
    }

    //写入文件尾
    av_write_trailer(oc);

    if(have_video)
    {
        close_stream(oc,&video_st);
    }

    if(have_audio)
    {
        close_stream(oc,&audio_st);
    }

    if(!(fmt->flags & AVFMT_NOFILE))
        avio_closep(&oc->pb);

    avformat_free_context(oc);
}


const int bit_rate = 64000;
const int sample_rate = 44100;
const int stream_frame_rate = 25;

/* Add an Output stream */
static void addStream(OutputStream *ost,AVFormatContext *oc,AVCodec **codec, enum AVCodecID codec_id)
{
    AVCodecContext *c = NULL;
    int i;
    /* 1 find a registered encoder  with a matching AVCodeID */
    AVCodec *tmpCodec;
    tmpCodec = avcodec_find_encoder(codec_id);
    *codec = tmpCodec;
    if(!(*codec))
    {
        fprintf(stderr,"Con't find encoder for '%s'\n ",avcodec_get_name(codec_id));
        exit(1);
    }
    ost->st = avformat_new_stream(oc,NULL);
    if (!ost->st){
        fprintf(stderr,"Could not allocate stream\n");
        exit(1);
    }
    ost->st->id = oc->nb_streams - 1;

    //通过AVCoder 分配 AVCodecContext
    c = avcodec_alloc_context3(*codec);
    if(!c){
        fprintf(stderr,"Could not alloc an encoding context\n");
        exit(1);
    }
    ost->enc = c;

    switch (tmpCodec->type) {
        case AVMEDIA_TYPE_AUDIO:
        {
            c->sample_fmt = tmpCodec->sample_fmts? tmpCodec->sample_fmts[0]:AV_SAMPLE_FMT_FLTP;
            c->bit_rate = bit_rate; //比特率
            c->sample_rate = sample_rate; //音频采样率

            //TODO: 这个似乎没有意义，强行设计samplerate 为 44100？
            if(tmpCodec->supported_samplerates)
            {
                c->sample_rate = tmpCodec->supported_samplerates[0];
                for(int i = 0;tmpCodec->supported_samplerates[i];i++)
                {
                    if(tmpCodec->supported_samplerates[i] == sample_rate){
                        c->sample_rate = sample_rate;
                    }
                }
            }
            c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
            c->channel_layout = AV_CH_LAYOUT_STEREO;

            //声道相关的设置
            if(tmpCodec->channel_layouts)
            {
                c->channel_layout = tmpCodec->channel_layouts[0];
                for(int i = 0;tmpCodec->channel_layouts[i];i++)
                {
                    if(tmpCodec->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                    {
                        c->channel_layout = AV_CH_LAYOUT_STEREO;
                    }
                }
            }

            c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
            ost->st->time_base = (AVRational){1,c->sample_rate};

        }break;

        case AVMEDIA_TYPE_VIDEO:{
            c->codec_id = codec_id;
            c->bit_rate = bit_rate;
            c->width = 352;
            c->height = 288;

            ost->st->time_base = (AVRational){1,stream_frame_rate};
            c->time_base = ost->st->time_base;

            c->gop_size = 12;
            c->pix_fmt = AV_PIX_FMT_YUV420P;

            if(c->codec_id == AV_CODEC_ID_MPEG2VIDEO)
            {
                c->max_b_frames = 2;
            }
            if(c->codec_id == AV_CODEC_ID_MPEG1VIDEO)
            {
                c->mb_decision = 2;
            }

        }break;
        default:
            break;
    }

}

static void open_video(AVFormatContext* oc,AVCodec* codec,OutputStream *ost,AVDictionary *opt_arg)
{
    int ret = -1;
    AVCodecContext *codecContext = ost->enc;
    AVDictionary *opt = NULL;
    av_dict_copy(&opt,opt_arg,0);

    //初始化解码器上下文
    ret = avcodec_open2(codecContext,codec,&opt);
    av_dict_free(&opt);
    if(0 != ret)
    {
        fprintf(stderr,"Could not open video codec:%s\n",av_err2str(ret));
        exit(-1);
    }
    //allocate and init re-usable frame,frame 存放解码后的单帧数据
    ost->frame = alloc_picture(codecContext->pix_fmt,codecContext->width,codecContext->height);
    if(!ost->frame)
    {
        fprintf(stderr,"Could not allloc video frame. \n");
        exit(-1);
    }


    //如果当前不是yur420p，则tmpframe yuv420p，转化成yuv420p格式

    ost->tmp_frame = NULL;
    if(codecContext->pix_fmt != AV_PIX_FMT_YUV420P)
    {
        ost->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P,
                                       codecContext->width,codecContext->height);
        if(!ost->tmp_frame)
        {
            fprintf(stderr,"Could allocate temporary picture.\n");
        }
    }

    //copy the stream parameters to the muxer.
    ret = avcodec_parameters_from_context(ost->st->codecpar,codecContext);
    if(0 != ret)
    {
        fprintf(stderr,"Could not copy the stream parameters. \n");
        exit(-1);
    }
}

static void open_audio(AVFormatContext* oc,AVCodec* codec,OutputStream *ost,AVDictionary *opt_arg)
{
    /*惯例，实用codec 初始化上下文*/
    AVCodecContext* c = ost->enc;
    int ret = -1;
    int nb_samples = 0;
    AVDictionary *opt;
    av_dict_copy(&opt,opt_arg,0);
    ret = avcodec_open2(c,codec,&opt);
    av_dict_free(&opt);
    if(0 != ret)
    {
        fprintf(stderr,"Could not open audio codec: %s \n",av_err2str(ret));
        exit(1);
    }

    ost->t = 0;
    ost->tincr = 2* M_PI *110.0 / c->sample_rate;
    ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    if(c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nb_samples = 10000;
    else
        nb_samples = c->frame_size;

    ost->frame = alloc_audio_frame(c->sample_fmt,c->channel_layout,c->sample_rate,nb_samples);
    ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16,c->channel_layout,
                                       c->sample_rate,nb_samples);

    ret = avcodec_parameters_from_context(ost->st->codecpar,c);
    if(0 != ret)
    {
        fprintf(stderr,"Counld not copy parameters \n");
        exit(-1);
    }

    //create resampler context
    ost->swr_ctx = swr_alloc();
    if(!ost->swr_ctx)
    {
        fprintf(stderr,"Could not allocate resampler context\n");
        exit(1);
    }

    av_opt_set_int(ost->swr_ctx,"in_channel_count",c->channels,0);
    av_opt_set_int(ost->swr_ctx,"in_sample_rate",c->sample_rate,0);
    av_opt_set_int(ost->swr_ctx,"in_sample_fmt",AV_SAMPLE_FMT_S16,0);
    av_opt_set_int(ost->swr_ctx,"out_channel_count",c->channels,0);
    av_opt_set_int(ost->swr_ctx,"out_sample_rate",c->sample_rate,0);
    av_opt_set_int(ost->swr_ctx,"out_sample_fmt",c->sample_fmt,0);

    if(swr_init(ost->swr_ctx) != 0)
    {
        fprintf(stderr,"Could not initialize the resampling context\n");
        exit(-1);
    }
}









static void findStreamInfo(AVFormatContext* oc)
{
    int ret = avformat_find_stream_info(oc,NULL);
    if(0 != ret)
    {
        fprintf(stderr,"Could not fund stream.\n");
    }
}

//mark: - allocate audio and video frame
/*allocate frame and not dealloc*/
static AVFrame* alloc_picture(enum AVPixelFormat pix_fmt,int width,int height)
{
    AVFrame *picture = av_frame_alloc();
    int ret = -1;
    if(!picture)
    {
        fprintf(stderr,"wearing : could not allocate AVFrame!");
        return NULL;
    }
    picture->format = pix_fmt;
    picture->height = height;
    picture->width = width;
    ret = av_frame_get_buffer(picture,0);
    if(0 != ret)
    {
        fprintf(stderr,"Could not allocte frame data. \n");
        exit(-1);
    }
    return picture;
}

/*audio output*/
static AVFrame* alloc_audio_frame(enum  AVSampleFormat sample_fmt,
                                  uint64_t channel_layout,
                                  int sample_rate,int nb_samples)
{
    AVFrame *frame = av_frame_alloc();
    int ret = -1;
    if(!frame)
    {
        fprintf(stderr,"Could not allocate an audio frame\n");
        exit(-1);
    }

    frame->format = sample_fmt;
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;

    if(nb_samples)
    {
        ret = av_frame_get_buffer(frame,0);
        if(0 != ret)
        {
            fprintf(stderr,"Could not allocating an audio buffer\n");
            fprintf(stderr,"error : %s \n",av_err2str(ret));
            exit(1);
        }
    }
    return frame;
}

static void log_packet(const AVFormatContext* fmt_ctx,const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

static  int write_frame(AVFormatContext* oc,AVCodecContext *c,AVStream *st,AVFrame *frame)
{
    int ret = -1;

    // 编码 encode;
    ret = avcodec_send_frame(c,frame);
    if(0 == ret)
    {
        fprintf(stderr,"Error sending a frame to encoder: %s\n",av_err2str(ret));
        exit(1);
    }

    while (ret >= 0)
    {
        AVPacket  pkt = {0};
        ret = avcodec_receive_packet(c,&pkt);
        if( ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            break;
        }else if ( ret < 0 ){
            fprintf(stderr,"Error encoding a frame: %s \n",av_err2str(ret));
            exit(1);
        }

        av_packet_rescale_ts(&pkt,c->time_base,st->time_base);

        pkt.stream_index =st->index;

        log_packet(oc,&pkt);
        ret = av_interleaved_write_frame(oc,&pkt);
        av_packet_unref(&pkt);
        if(0 != ret)
        {
            fprintf(stderr,"Error while writer output packet: %s \n",av_err2str(ret));
            exit(1);
        }

    }
    return ret == AVERROR_EOF ? 1:0;
}



/* prepare a dummy image. */
static void fill_yuv_image(AVFrame* pict,int frame_index,int width,int height)
{
    // YUV420
    int x,y,i;
    i = frame_index;

    /* Y */
    /* 遍历每个像素点 */

    for(y =0; y < height;y++)
    {
        for(x = 0;x <width;x++)
        {
            pict->data[0][y * pict->linesize[0] + 1] = x + y + i * 3;
        }
    }

    /* Cb and Cr */
    for(y = 0; y < height/2; y++)
    {
        for(x = 0; x < width/2; x++)
        {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }

}

static  AVFrame* get_video_frame(OutputStream *ost){
    AVCodecContext  *c = ost->enc;
    if(av_compare_ts(ost->next_pts,c->time_base,Stream_duration,(AVRational){1,1})>0)
    {
        return NULL;
    }
    //check frame can be use
    if(av_frame_make_writable(ost->frame) < 0)
    {
        exit(1);
    }

    if(c->pix_fmt != AV_PIX_FMT_YUV420P){
        if (!ost->sws_ctx)
        {
            ost->sws_ctx = sws_getContext(c->width,c->height,AV_PIX_FMT_YUV420P,
                                          c->width,c->height,
                                          c->pix_fmt,SWS_BICUBIC,NULL,
                                          NULL,NULL);
            if(!ost->sws_ctx){
                fprintf(stderr,"Could not initialize the conversion context\n");
                exit(1);
            }
        }
        fill_yuv_image(ost->tmp_frame,ost->next_pts,c->width,c->height);
        sws_scale(ost->sws_ctx,(const uint8_t * const *)ost->tmp_frame->data,
                  ost->tmp_frame->linesize,0,c->height,
                  ost->frame->data,ost->frame->linesize);


    }else
    {
        fill_yuv_image(ost->frame,ost->next_pts,c->width,c->height);
    }
    ost->frame->pts = ost->next_pts++;
    return ost->frame;

}


static  int write_video_frame(AVFormatContext *oc,OutputStream *ost)
{
    return write_frame(oc,ost->enc,ost->st,get_video_frame(ost));
}


static  AVFrame* getAudioFrame(OutputStream *ost)
{
    AVFrame *frame = ost->tmp_frame;
    int j,i,v;
    int16_t  *q = (int16_t*)frame->data[0];

    if(av_compare_ts(ost->next_pts,ost->enc->time_base,
                     Stream_duration,(AVRational){1,1})>0)
    {
        return NULL;
    }

    for(j = 0; j < frame->nb_samples;j++)
    {
        v = (int)sin(ost->t)*1000;
        for(i=0;i < ost->enc->channels;i++)
        {
            *q++ =v;
        }
        ost->t += ost->tincr;
        ost->tincr += ost->tincr2;
    }

    frame->pts = ost->next_pts;
    ost->next_pts += ost->frame->nb_samples;
    return frame;
}

static  int write_audio_frame(AVFormatContext *oc,OutputStream *ost)
{

    AVCodecContext *c = ost->enc;
    AVFrame *frame = getAudioFrame(ost);
    int ret = -1;
    int dst_nb_samples = -1;

    if (frame)
    {
        dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx,c->sample_rate) + frame->nb_samples,
                                        c->sample_rate,c->sample_rate,AV_ROUND_UP);
        av_assert0(dst_nb_samples == frame->nb_samples);

        ret = av_frame_make_writable(frame);
        if(0 != ret){
            fprintf(stderr,"Could not writable audio frame");
            exit(1);
        }

        ret = swr_convert(ost->swr_ctx,ost->frame->data,dst_nb_samples,
                          (const uint8_t **)frame->data,frame->nb_samples);
        if(0 != ret){
            fprintf(stderr,"Error while coverting\n");
            exit(1);
        }
        frame = ost->frame;
        frame->pts = av_rescale_q(ost->samples_count,
                                  (AVRational){1,c->sample_rate},
                                  c->time_base);
        ost->samples_count += dst_nb_samples;
    }

    return write_frame(oc,c,ost->st,frame);
}

static void close_stream(AVFormatContext *oc,OutputStream* ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}
#endif