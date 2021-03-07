////
//// Created by dong wu on 2021/3/4.
////

#include "GifParser.h"

std::ostream &operator<<(std::ostream &os, const GifHeader &d) {
    os << "GifHeader("
       << "position=[" << d.beginPosition << "," << d.endPosition << "],"
       << "signature=" << d.name << ", version=" << d.version << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const LogicalScreenDescriptor &d) {
    os << " LogicalScreenDescriptor(" << std::endl
       << "  position=[" << d.beginPosition << "," << d.endPosition << "],\n"
       << "  logicalScreenWidth=" << d.logicalWidth << ",\n"
       << "  logicalScreenHeight=" << d.logicalHeight << ",\n"
       << std::hex << std::showbase //
       << "  packedFields=" << +d.packedFiled << ",\n"
       << std::resetiosflags(std::ios::hex | std::ios::showbase) //
       << "    [p]globalColorTableFlag=" << d.globalTableColorFlag << ",\n"
       << "    [p]colorResolution=" << +d.colorResolution << ",\n"
       << "    [p]sortFlag=" << d.sortFlag << ",\n"
       << "    [p]sizeOfGlobalColorTable=" << +d.sizeOfFGlobalTableColor << ",\n"
       << "  backgroundColorIndex=" << +d.backgroundColorIndex << ",\n"
       << "  pixelAspectRatio=" << +d.pixelAspectRation << ",\n"
       << ",\n"
       << " )";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ColorTable &d) {
    os << "ColorTable("
       << "position=[" << d.beginPosition << "," << d.endPosition << "],"
       << "size=" << d.colors.size() //
       << ")";
    return os;
}


std::ostream &operator<<(std::ostream &os, const LogicalScreen &d) {
    os << "LogicScreen(" << std::endl
       << "position=[" << d.beginPosition << "," << d.endPosition << "], "
       << std::endl
       << d.descriptor << std::endl
       << d.golobarColorTable << std::endl
       << ")";
    return os;
}


std::ostream &operator<<(std::ostream &os, const ApplicationExtension &d) {
    os << "ApplicationExtension(" << std::endl
       << "  position=[" << d.beginPosition << "," << d.endPosition << "],\n"
       << std::hex << std::showbase //
       << "  extensionIntroducer=" << +d.applicationIntroducer << ",\n"
       << "  extensionLabel=" << +d.applicationExtensionLabel << ",\n"
       << std::resetiosflags(std::ios::hex | std::ios::showbase) //
       << "  blockSize=" << +d.size << ",\n"
       << "  applicationIdentifier=" << d.applicationIndentifier << ",\n"
       << "  applAuthenticationCode=" << d.applAuthenticationCode << ",\n"
       << "  ApplicationData=(size=" << d.applicationData.size() << ",\n"
       << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const SubBlock &d) {
    os << "SubBlock("
       << "position=[" << d.beginPosition << "," << d.endPosition << "],"
       << "size=" << +d.size; //
    os << std::hex << std::showbase;
    for (const auto &u : d.data) {
        os << static_cast<int>(u);
    }
    os << std::resetiosflags(std::ios::hex | std::ios::showbase) //
       << ")";
    return os;
}


std::ostream &operator<<(std::ostream &os, const CommentExtension &d) {
    os << "CommentExtension("
       << "position=[" << d.beginPosition << "," << d.endPosition << "]"
       << ")" << std::endl;
    return os;
}



std::ostream &operator<<(std::ostream &os, const GraphicControlExtension &d) {
    os << "  GraphicControlExtension(" << std::endl
       << "    position=[" << d.beginPosition << "," << d.endPosition << "],\n"
       << std::hex << std::showbase
       << "    extensionIntroducer=" << +d.introducer << ",\n"
       << "    graphicControlLabel=" << +d.GraphicControLabel << ",\n"
       << std::resetiosflags(std::ios::hex | std::ios::showbase)
       << "    blockSize=" << +d.blockSize << ",\n"                         //
       << "    delayTime=" << +d.delayTime << ",\n"                         //
       << "    transparentColorIndex=" << +d.transparentColorIndex << ",\n" //
       << std::hex << std::showbase                                         //
       << "    packedFields=" << +d.packedFields << ",\n"                   //
       << "      [p]reserved=" << +d.reserved << ",\n"
       << "      [p]disposalMethod=" << +d.disposalMethod << ",\n"
       << "      [p]userInputFlag=" << d.userInputFlag << ",\n"
       << "      [p]transparentColorFlag=" << +d.tranparentColorFlag << ",\n"
       << std::resetiosflags(std::ios::hex | std::ios::showbase) //
       << "  )";
    return os;
}

std::ostream &operator<<(std::ostream &os, const PlainTextExtension &d) {
    os << "PlainTextExtension("
       << "position=[" << d.beginPosition << "," << d.endPosition << "])";

    return os;
}

std::ostream &operator<<(std::ostream &os, const ImageDescriptor &d) {
    os << "    ImageDescriptor(" << std::endl
       << "      position=[" << d.beginPosition << "," << d.endPosition
       << "],\n"
       << std::hex << std::showbase
       << "      imageSeparator=" << +d.imageSeparator << ",\n"
       << std::resetiosflags(std::ios::hex | std::ios::showbase)
       << "      imageLeftPosition=" << +d.leftPosition << ",\n"
       << "      imageTopPosition=" << +d.topPosition << ",\n"
       << "      imageWidth=" << +d.imageWidth << ",\n"
       << "      imageHeight=" << +d.imageHeight << ",\n"
       << std::hex << std::showbase                              //
       << "      packedFields=" << +d.packfields << ",\n"      //
       << std::resetiosflags(std::ios::hex | std::ios::showbase) //
       << "        [p]localColorTableFlag=" << d.localColorTableFlag << ",\n"
       << "        [p]interlaceFlag=" << d.interlaceFlag << ",\n"
       << "        [p]sortFlag=" << d.sortFlag << ",\n"
       << std::hex << std::showbase //
       << "        [p]reserved=" << +d.reserved << ",\n"
       << std::resetiosflags(std::ios::hex | std::ios::showbase) //
       << "        [p]sizeOfLocalColorTable=" << +d.sizeofLocalTable
       << ",\n"
       << "  )";

    return os;
}



std::ostream &operator<<(std::ostream &os, const TableBaseImageData &d) {
    os << "TableBasedImageData("
       << "position=[" << d.beginPosition << "," << d.endPosition << "], "
       << "lzwMinimumCodeSize=" << +d.minimunCodeSize << ","
       << "ImageData=(size=" << d.imageData.size() << ")";

    os << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const TableBasedImage &d) {
    os << "    TableBasedImage(" << std::endl
       << "      position=[" << d.beginPosition << "," << d.endPosition << "]\n"
       << d.imageDescriptor << ", " << d.localColorTable << ", " << d.imageData
       << ")";
    return os;
}


struct GraphicBlock{

    GraphicControlExtension graphicControlExtension;
    GraphicRenderingBlock graphicRenderingBlock;

    int beginPosition;
    int endPosition;

    std::istream &parse(std::istream &input) {
        beginPosition = static_cast<int>(input.tellg()) + 1;
        graphicControlExtension.parse(input);
        graphicRenderingBlock.parse(input);
        endPosition = input.tellg();
        return input;
    }


};


std::ostream &operator<<(std::ostream &os, const GraphicRenderingBlock &d) {
    os << "  GraphicRenderingBlock(" << std::endl;
    os << "    position=[" << d.beginPosition << "," << d.endPosition << "]"
       << std::endl;
    if (d.type == GraphicRenderingBlock::Type::PlainRTextExtensoin) {
        os << d.plainTextExtension;
    } else {
        os << d.tableBasedImage;
    }
    os << "  )";
    return os;
}


std::ostream &operator<<(std::ostream &os, const GraphicBlock &d) {
    os << "GraphicBlock(" //
       << std::endl       //
       << "  position=[" << d.beginPosition << "," << d.endPosition << "]"
       << std::endl //
       << d.graphicControlExtension << std::endl
       << d.graphicRenderingBlock << std::endl
       << ")";
    return os;
}

struct Trailer {
    char trailer;
    int beginPosition;
    int endPosition;
    std::istream &parse(std::istream &input) {
        beginPosition = static_cast<int>(input.tellg()) + 1;
        input.read(reinterpret_cast<char *>(&trailer), sizeof(trailer));
        endPosition = input.tellg();
        return input;
    }
};



GifParser::GifParser() {
   filePath = "123.gif";
}

GifParser::GifParser(std::string fileName):filePath(fileName)
{

}

void GifParser::parse()
{
    std::ifstream input(filePath,std::ios::in);
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
    Trailer trailer;

    int beginPosition;
    int endPosition;

    std::vector<GraphicBlock> graphicBlocks;
    ApplicationExtension applicationExtension;
    CommentExtension commentExtension;

    //图形控制扩展块 标志是 0x21
    while (input.peek() == 0x21){
        input.get();
        uint8_t extensionLabel = input.get();
        input.unget();
        input.unget();

        if (extensionLabel == 0xff)
        {
            applicationExtension.parse(input);
            std::cout<<applicationExtension<<std::endl;
        }else if (extensionLabel == 0xfe)  //注释拓展块
        {
            commentExtension.parse(input);
            std::cout<<commentExtension<<std::endl;
        }else if (extensionLabel == 0xf9)   //图形控制拓展块
        {
            GraphicBlock graphicBlock;
            graphicBlock.parse(input);
            std::cout<<graphicBlock<<std::endl;
            graphicBlocks.push_back(graphicBlock);
        }else{
            break;
        }
    }

    if (input.peek() == 0x3B) {
        trailer.parse(input);
        endPosition = input.tellg();
    } else {
        std::cout << "Error"                   //
                  << std::hex << std::showbase //
                  << +input.peek()             //
                  << std::resetiosflags(std::ios::hex |
                                        std::ios::showbase) //
                  << std::endl;                             //
    }

    std::cout << "frame counts: " << graphicBlocks.size() << std::endl;

    //graphicBlocks.at(0).graphicRenderingBlock.tableBasedImage.imageData.write(0);

}