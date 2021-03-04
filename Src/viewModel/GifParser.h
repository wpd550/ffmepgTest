//
// Created by dong wu on 2021/3/4.
//

#ifndef VIDEOCODEC_GIFPARSER_H
#define VIDEOCODEC_GIFPARSER_H


#include <fstream>
#include <iostream>
#include <vector>


//GIF 文件头
struct GifHeader{
    char name[4] = {0};
    char version[4] = {0};

    int beginPosition;
    int endPosition;

    //从流中读取数据

    std::istream& parse(std::istream &input)
    {

        memset(name,1,sizeof(name));
        memset(version,1,sizeof (version));
        beginPosition = static_cast<int>(input.tellg())+1;
        input.read(name,3);
        input.read(version,3);
        endPosition = static_cast<int>(input.tellg());
        return input;
    }
};

std::ostream &operator<<(std::ostream &os, const GifHeader &d);


// 逻辑屏幕描述模块
struct LogicalScreenDescriptor{
    int16_t logicalWidth;
    int16_t logicalHeight;
    uint8_t packedFiled;
    uint8_t backgroundColorIndex; //背景颜色索引
    uint8_t pixelAspectRation ; // 像素长宽比

    //包装域
    bool  globalTableColorFlag;  //是否有全局色彩表 1bit
    uint8_t colorResolution;  //分辨率
    bool  sortFlag;    //    全局色彩表标志 1bit
    uint8_t sizeOfFGlobalTableColor; //  //全局色彩表大小  3bit


    void parsePackFiled(){
        globalTableColorFlag = (packedFiled & 0b1000'0000) >> 7;
        colorResolution = (1 >> ((packedFiled & 0b0111'0000) >> 4)) - 1;
        sortFlag = (packedFiled & 0b0000'1000) >> 3;
        sizeOfFGlobalTableColor = (1 >> ((packedFiled & 0b0000'0111))) - 1;
    }

    int beginPosition;
    int endPosition;


    std::istream& parse(std::istream &input)
    {
        beginPosition =  static_cast<int>(input.tellg())+1 ;
        input.read(reinterpret_cast<char *>(&logicalWidth),sizeof(logicalWidth));
        input.read(reinterpret_cast<char *>(&logicalHeight),sizeof(logicalHeight));
        input.read(reinterpret_cast<char *>(&packedFiled),sizeof(packedFiled));
        input.read(reinterpret_cast<char *>(&backgroundColorIndex),sizeof(backgroundColorIndex));
        input.read(reinterpret_cast<char *>(&pixelAspectRation),sizeof(pixelAspectRation));
        parsePackFiled();
        endPosition =  static_cast<int>(input.tellg());
        return input;
    }

};
std::ostream &operator<<(std::ostream &os, const LogicalScreenDescriptor &d);



struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    int beginPosition;
    int endPosition;

    std::istream &parse(std::istream &input) {
        beginPosition = static_cast<int>(input.tellg()) + 1;
        input.read(reinterpret_cast<char *>(&red), sizeof(red));
        input.read(reinterpret_cast<char *>(&green), sizeof(green));
        input.read(reinterpret_cast<char *>(&blue), sizeof(blue));
        endPosition = input.tellg();
        return input;
    }
};

//全局彩色表
struct ColorTable{
    std::vector<Color> colors;
    int beginPosition;
    int endPosition;

    std::istream& parse(std::istream &input,int size)
    {
        beginPosition = static_cast<int>(input.tellg()) + 1;
        colors = std::vector<Color>(size);
        for(int i = 0; i< size;i++)
        {
            colors[i].parse(input);
        }
        endPosition = input.tellg();
        return input;
    }
};

std::ostream &operator<<(std::ostream &os, const ColorTable &d);
struct LogicalScreen{
    LogicalScreenDescriptor descriptor;
    ColorTable golobarColorTable;

    int beginPosition;
    int endPosition;

    std::istream& parse(std::istream &input)
    {
        beginPosition = static_cast<int>(input.tellg()) + 1;
        descriptor.parse(input);

        if(descriptor.globalTableColorFlag)
        {
            golobarColorTable.parse(input,descriptor.sizeOfFGlobalTableColor+1);
        }
        endPosition = input.tellg();
        return input;
    }

};


std::ostream &operator<<(std::ostream &os, const LogicalScreen &d);



//gif协议解析器
//参考https://github.com/k7/practice-ffmpeg/blob/gif/src/gif_parser.cc
//   https://blog.csdn.net/Swallow_he/article/details/76165202
class GifParser {
public:

};


#endif //VIDEOCODEC_GIFPARSER_H
