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


int main(int argc, char* argv[]){
/*解码*/
//    run();
//    DemuxingAndDecodingFactor factor = DemuxingAndDecodingFactor(
//            "/Users/dong/Desktop/2020.mkv");


    MedieInfo info = MedieInfo(FILEPATH);
    info.run();

//    ffmpeg_decode(argc,argv);
    return 0;
}
