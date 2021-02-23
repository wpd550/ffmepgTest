//
// Created by dong on 2021/1/24.
//

#ifndef FFMPEG_PRECH_H
#define FFMPEG_PRECH_H


#ifdef __cplusplus
extern "C" {
#endif

#include <libavutil/channel_layout.h> //用户音频声道布局操作
#include <libavutil/opt.h>   //用于参数调试
#include <libavutil/mathematics.h>   //数学相关
#include <libavutil/timestamp.h>     //时间戳
#include <libavutil/avassert.h>
#include <libavformat/avformat.h>   //解封装 和 封装
#include <libswscale/swscale.h>    //缩放和颜色转化
#include <libswresample/swresample.h> //音频采样率的操作

#ifdef __cplusplus
}

#include <string>
#include <iostream>
#include <stdio.h>

#define DEBUGMRCO 1

#define FILEPATH "/Users/dong/Desktop/test2.mp4"

#endif
#endif //FFMPEG_PRECH_H
