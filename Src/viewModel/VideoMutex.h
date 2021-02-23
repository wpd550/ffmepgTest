//
// Created by dong on 2021/1/29.
//

#ifndef FFMPEG_VIDEOMUTEX_H
#define FFMPEG_VIDEOMUTEX_H

#include <string>
#include <iostream>

class VideoMutex {
public:
    VideoMutex(std::string fileName)
    {
        this->fileName = fileName;
//

        std::cerr<< "Initialize VideoModel \n";
    }

    ~VideoMutex() = default;
private:
    std::string fileName;

};


#endif //FFMPEG_VIDEOMUTEX_H
