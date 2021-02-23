//
// Created by dong on 2021/2/2.
//

#ifndef FFMPEG_DEMUXINGANDDECODINGFACTOR_H
#define FFMPEG_DEMUXINGANDDECODINGFACTOR_H

#include "../prech.h"


std::string lastPathComponent(std::string path);

std::string stringByDeletePathExtension(std::string path);

std::string pathExtension(std::string path);


class DemuxingAndDecodingFactor {
public:
    DemuxingAndDecodingFactor(std::string srcfileName):srcFileName(srcfileName){
        std::cout<<"extension = "<< pathExtension(srcfileName)<<std::endl;
        std::cout<<"stringDelete = "<< stringByDeletePathExtension(srcfileName)<<std::endl;
        std::cout<<"lastcomponent = "<< lastPathComponent(srcfileName) <<std::endl;
    }
private:
    std::string srcFileName;
    std::string dstAudioFileName;
    std::string dstVideoFileName;
};


#endif //FFMPEG_DEMUXINGANDDECODINGFACTOR_H
