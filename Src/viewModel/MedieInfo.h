//
// Created by dong on 2021/2/8.
//

#ifndef FFMPEG_MEDIEINFO_H
#define FFMPEG_MEDIEINFO_H

#include <string>
#include "base.h"
#include "../prech.h"

class MedieInfo : public base{
public:
    MedieInfo(std::string fileName):fileName(fileName){
//        this->run();
    }
    ~MedieInfo() = default;
    int run();

private:
    std::string fileName;

    int openCodeContext(int &streamIdx,AVCodecContext** pAvCodecContext,
                        AVFormatContext * pAvFormatContext,enum AVMediaType type);

    int videoIndex;
    int audioIndex;
};





#endif //FFMPEG_MEDIEINFO_H
