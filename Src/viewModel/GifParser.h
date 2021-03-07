//
// Created by dong wu on 2021/3/4.
//

#ifndef VIDEOCODEC_GIFPARSER_H
#define VIDEOCODEC_GIFPARSER_H


#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

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
        input.read(reinterpret_cast<char *>(&logicalWidth),
                   sizeof(logicalWidth));
        input.read(reinterpret_cast<char *>(&logicalHeight),
                   sizeof(logicalHeight));
        input.read(reinterpret_cast<char *>(&packedFiled),
                   sizeof(packedFiled));
        input.read(reinterpret_cast<char *>(&backgroundColorIndex),
                   sizeof(backgroundColorIndex));
        input.read(reinterpret_cast<char *>(&pixelAspectRation),
                   sizeof(pixelAspectRation));
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



// 保留block 数据
struct SubBlock{
    uint8_t  size;
    std::vector<uint8_t> data;

    int beginPosition;
    int endPosition;

    std::istream& parse(std::istream& input)
    {
        beginPosition = static_cast<int>(input.tellg())+1;
        input.read(reinterpret_cast<char*>(&size), sizeof(size));
        if(size)
        {
            data.resize(size);
            input.read(reinterpret_cast<char*>(data.data()),size);
        }
        endPosition = static_cast<int>(input.tellg());
        return input;
    }

    void writer(std::ostream &output){
        output<<data.data();
    }
};

std::ostream &operator<<(std::ostream &os, const SubBlock &d);

/*固定12字节 + block terminal*/
struct ApplicationExtension{
    uint8_t applicationIntroducer;
    uint8_t applicationExtensionLabel;
    uint8_t size;
    char applicationIndentifier[8] = {0};
    char applAuthenticationCode[3] = {0};

    //可能存在很多的block
    std::vector<SubBlock> applicationData;
//    SubBlock dataBlock;

    int beginPosition;
    int endPosition;

    std::istream& parse(std::istream  &input){
        beginPosition = static_cast<int>(input.tellg())+1;
        input.read(reinterpret_cast<char*>(&applicationIntroducer),
                   sizeof(applicationIntroducer));
        input.read(reinterpret_cast<char*>(&applicationExtensionLabel),
                   sizeof(applicationExtensionLabel));
        input.read(reinterpret_cast<char*>(&size),
                   sizeof(size));

        input.read(reinterpret_cast<char*>(applicationIndentifier),
                   sizeof(applicationIndentifier));
        input.read(reinterpret_cast<char*>(applAuthenticationCode),
                   sizeof(applAuthenticationCode));

        while (true)
        {
            SubBlock block;
            block.parse(input);
            if (block.size == 0){
                break;
            }
            applicationData.push_back(block);
        }
        endPosition = static_cast<int>(input.tellg());
        return input;
    }
};

std::ostream &operator<<(std::ostream &os, const ApplicationExtension &d);


struct CommentExtension{
    uint8_t  introducer;
    uint8_t  commentLabel;

    std::vector<SubBlock> commentBlock;

    int beginPosition;
    int endPosition;


    std::istream& parse(std::istream& input)
    {
        beginPosition = static_cast<int>(input.tellg())+1;

        input.read(reinterpret_cast<char*>(&introducer),
                   sizeof(introducer));
        input.read(reinterpret_cast<char*>(&commentLabel),
                   sizeof(commentLabel));

        while (true)
        {
            SubBlock block;
            block.parse(input);
            if (block.size == 0){
                break;
            }
            commentBlock.push_back(block);
        }
        endPosition = static_cast<int>(input.tellg());
        return input;
    }
};

std::ostream &operator<<(std::ostream &os, const CommentExtension &d);


struct GraphicControlExtension{
    uint8_t introducer;
    uint8_t GraphicControLabel; //固定为 0xf9
    uint8_t blockSize;
    uint8_t packedFields;
    uint16_t delayTime;
    uint8_t transparentColorIndex;

    uint8_t blockTerminator; //固定一个字节 0x00

    //packedFields
    uint8_t reserved;
    uint8_t disposalMethod;
    bool  userInputFlag;
    bool tranparentColorFlag;

    int beginPosition;
    int endPosition;
    std::istream& parse(std::istream& input){
        beginPosition = static_cast<int>(input.tellg()) + 1;
        input.read(reinterpret_cast<char*>(&introducer),
                   sizeof(introducer));
        input.read(reinterpret_cast<char*>(&GraphicControLabel),
                   sizeof(GraphicControLabel));
        input.read(reinterpret_cast<char*>(&blockSize),
                   sizeof(blockSize));
        input.read(reinterpret_cast<char*>(&packedFields),
                   sizeof(packedFields));
        input.read(reinterpret_cast<char*>(&delayTime),
                   sizeof(delayTime));
        input.read(reinterpret_cast<char*>(&transparentColorIndex),
                    sizeof(transparentColorIndex));
        input.read(reinterpret_cast<char*>(&blockTerminator),
                   sizeof(blockTerminator));
        endPosition = static_cast<int>(input.tellg());
        parsePackFields();
        return input;

    }

    void parsePackFields(){
        tranparentColorFlag = (packedFields & 0b00000001);
        userInputFlag       = (packedFields & 0b00000010) >>1;
        disposalMethod      = (packedFields & 0b00011100) >>2;
        reserved            = (packedFields & 0b11100000) >>5;
    }
};

std::ostream &operator<<(std::ostream &os, const GraphicControlExtension &d);

/** 一般都不使用这个extension 了  **/
struct PlainTextExtension{
    uint8_t introducer;
    uint8_t plainTextLabel;
    uint8_t blockSize;

    uint16_t leftPosition;
    uint16_t topPosition;
    uint16_t gridWidth;
    uint16_t gridHeight;

    uint8_t cellWidth;
    uint8_t cellheight;
    uint8_t textForgroundColorIndex;
    uint8_t textBackgoundColorIndex;

    std::vector<SubBlock>plainTextData;

    int beginPosition;
    int endPosition;

    std::istream& parse(std::istream& input)
    {
        beginPosition = static_cast<int>(input.tellg()) + 1;
        endPosition = input.tellg();
        return input;
    }

};

struct ImageDescriptor{
    uint8_t imageSeparator;
    uint16_t  leftPosition;
    uint16_t  topPosition;
    uint16_t  imageWidth;
    uint16_t  imageHeight;

    uint8_t  packfields;
    int beginPosition;
    int endPosition;

    bool  localColorTableFlag;
    bool  interlaceFlag;
    bool  sortFlag;
    uint8_t reserved;
    uint8_t sizeofLocalTable;

    std::istream& parse(std::istream& input){
        beginPosition = static_cast<int>(input.tellg()) + 1;

        input.read(reinterpret_cast<char*>(&imageSeparator),
                   sizeof(imageSeparator));
        input.read(reinterpret_cast<char*>(&leftPosition),
                   sizeof(leftPosition));
        input.read(reinterpret_cast<char*>(&topPosition),
                   sizeof(topPosition));
        input.read(reinterpret_cast<char*>(&imageWidth),
                   sizeof(imageWidth));
        input.read(reinterpret_cast<char*>(&imageHeight),
                   sizeof(imageHeight));
        input.read(reinterpret_cast<char*>(&packfields),
                   sizeof(packfields));

        parsePackfields();

        endPosition = input.tellg();
        return input;

    }
    void parsePackfields()
    {
        localColorTableFlag = (packfields & 0b10000000) >> 7;
        interlaceFlag       = (packfields & 0b01000000) >> 6;
        sortFlag            = (packfields & 0b00100000) >> 5;
        reserved            = (packfields & 0b00011000) >> 3;
        sizeofLocalTable    = (1 >> (packfields & 0b00000111)) - 1;
    }
};

// LZW 压缩算法
struct TableBaseImageData{
    uint8_t minimunCodeSize;

    std::vector<SubBlock> imageData;

    int beginPosition;
    int endPosition;

    std::istream& parse(std::istream &input)
    {
        beginPosition = static_cast<int>(input.tellg()) + 1;
        input.read(reinterpret_cast<char*>(&minimunCodeSize),
                   sizeof(minimunCodeSize));
        int i = 0;

        while (true)
        {
            i++;
            SubBlock block;
            block.parse(input);
            if(block.size == 0)
            {
                break;
            }

            imageData.push_back(block);
        }
        endPosition = static_cast<int>(input.tellg());
        return input;
    }


    void write(int number)
    {
        std::stringstream  ss;
        std::string fileName = "lzw";

        ss<<fileName<<number<<".lzw";

        std::ofstream  output(ss.str(),std::ios::out);
        for (SubBlock block :imageData)
        {
            block.writer(output);
        }
        output.close();
    }
};

/*
 * 1. imageDescriptor
 * 2. localColorTable
 * 3. imageData
 * */
struct TableBasedImage {
    ImageDescriptor imageDescriptor;
    ColorTable localColorTable;
    TableBaseImageData imageData;

    int beginPosition;
    int endPosition;

    std::istream& parse(std::istream &input)
    {
        beginPosition = static_cast<int>(input.tellg()) + 1;
        imageDescriptor.parse(input);
        if(imageDescriptor.localColorTableFlag){
            localColorTable.parse(input,imageDescriptor.sizeofLocalTable);
        }
        imageData.parse(input);
        endPosition = static_cast<int>(input.tellg());
        return input;
    }
};





struct GraphicRenderingBlock
{
    enum class Type{
        Unknow,
        PlainRTextExtensoin,
        TableBaseedImage,
    };

    PlainTextExtension plainTextExtension;
    TableBasedImage tableBasedImage;

    Type type = Type::Unknow;

    int beginPosition;
    int endPosition;

    std::istream& parse(std::istream &input)
    {
        beginPosition = static_cast<int>(input.tellg()) + 1;

        uint8_t extensionIntroducer = input.get();
        uint8_t extensionLabel = input.get();
        input.unget();
        input.unget();

        //很少用 PlainRTextExtensoin
        if (extensionIntroducer == 0x21 && extensionLabel == 0x01) {
            type = Type::PlainRTextExtensoin;
            plainTextExtension.parse(input);
        } else {
            type = Type::TableBaseedImage;
            tableBasedImage.parse(input);
        }
        endPosition = input.tellg();
        return input;
    }
};
std::ostream &operator<<(std::ostream &os, const GraphicRenderingBlock &d);





//gif协议解析器
//参考https://github.com/k7/practice-ffmpeg/blob/gif/src/gif_parser.cc
//   https://blog.csdn.net/Swallow_he/article/details/76165202
class GifParser {
public:
    GifParser();
    GifParser(std::string fileName);

    void parse();

private:
    std::string filePath;

};


#endif //VIDEOCODEC_GIFPARSER_H
