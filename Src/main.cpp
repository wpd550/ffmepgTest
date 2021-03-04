#include <iostream>
#include "prech.h"


#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include "viewModel/mutex.h"

#include "prech.h"

#include "viewModel/VideoMutex.h"

#include "viewModel/DemuxingAndDecodingFactor.h"

#include "viewModel/MedieInfo.h"

#include "viewModel/simpleDecode.h"

#include <fstream>
#include "viewModel/GifParser.h"



void gifParseTest(){

    std::ifstream input("123.gif",std::ios::in);
    if(!input)
    {
        std::cerr<<"open ifstream"<<"123.gif"<<"failed"<<std::endl;
    }
    GifHeader header;
    header.parse(input);
    std::cout<<header<<std::endl;

    LogicalScreen logicalScreen;
    logicalScreen.parse(input);

    std::cout<<logicalScreen<<std::endl;

}

int main(int argc, char* argv[]){

    gifParseTest();
/*解码*/

//    run();
//    DemuxingAndDecodingFactor factor = DemuxingAndDecodingFactor(
//            "/Users/dong/Desktop/2020.mkv");


//    MedieInfo info = MedieInfo(FILEPATH);
//    info.run();

//    ffmpeg_decode(argc,argv);
    return 0;
}
