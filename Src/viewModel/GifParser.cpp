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
       << "  blockSize=" << d.size << ",\n"
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






GifParser::GifParser() {
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

    //图形控制扩展块 标志是 0x21
    while (input.peek() == 0x21){
        input.get();
        uint8_t extensionLabel = input.get();
        input.unget();
        input.unget();

        switch (extensionLabel) {
            case 0xff:   //ApplicationExtension
            {
                ApplicationExtension applicationExtension;
                applicationExtension.parse(input);
                std::cout<<applicationExtension<<std::endl;
            }break;
            case 0xfe:
            {

            }
            default:{

            }
        }

    }

}