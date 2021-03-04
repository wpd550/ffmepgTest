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
