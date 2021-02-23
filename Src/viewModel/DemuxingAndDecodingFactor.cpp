//
// Created by dong on 2021/2/2.
//

#include "DemuxingAndDecodingFactor.h"


std::string lastPathComponent(std::string path)
{
    std::string::size_type  iPos=  path.find_last_of("/")+1;
    std::string fileName = path.substr(iPos,path.length() -iPos);
    std::cout<< "srcFileName = "<<fileName<<std::endl;
    return fileName;
}

std::string stringByDeletePathExtension(std::string path)
{
    return path.substr(0,path.rfind("."));
}

std::string pathExtension(std::string path)
{
    return path.substr(path.find_last_of(".") + 1);
}
