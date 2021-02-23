cmake_minimum_required(VERSION 3.17)
project(ffmpeg)
set(ffmpeg_VERSION_MAJOR 1.0.0)
set(ffmpeg_VERSION_MINOR 1.0.0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Bin")


include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)



option(USE_MACRO
        "Build the project using macro"
        OFF)
IF(USE_MACRO)
    add_definitions("-DUSE_MACRO")
endif(USE_MACRO)

add_executable(ffmpeg Src/main.cpp Src/prech.h Src/viewModel/VideoMutex.cpp Src/viewModel/VideoMutex.h Src/viewModel/mutex.h Src/viewModel/DemuxingAndDecodingFactor.cpp Src/viewModel/DemuxingAndDecodingFactor.h Src/viewModel/MedieInfo.cpp Src/viewModel/MedieInfo.h Src/viewModel/base.cpp Src/viewModel/base.h Src/viewModel/simpleDecode.h)

find_library(rs_Lib  NAMES libavformat.58.65.101.dylib HINT ${PROJECT_SOURCE_DIR}/lib )
message("libpath =  ${rs_lib}")
# target_link_libraries 需要在add_executable 之后
target_link_libraries(ffmpeg ${PROJECT_SOURCE_DIR}/lib/libavformat.58.65.101.dylib )
target_link_libraries(ffmpeg ${PROJECT_SOURCE_DIR}/lib/libswscale.5.8.100.dylib)
target_link_libraries(ffmpeg ${PROJECT_SOURCE_DIR}/lib/libavcodec.58.119.100.dylib)
target_link_libraries(ffmpeg ${PROJECT_SOURCE_DIR}/lib/libavdevice.58.11.103.dylib)
target_link_libraries(ffmpeg ${PROJECT_SOURCE_DIR}/lib/libavfilter.7.98.100.dylib)
target_link_libraries(ffmpeg ${PROJECT_SOURCE_DIR}/lib/libavutil.56.64.100.dylib)
target_link_libraries(ffmpeg ${PROJECT_SOURCE_DIR}/lib/libmp3lame.0.dylib)
target_link_libraries(ffmpeg ${PROJECT_SOURCE_DIR}/lib/libswresample.3.8.100.dylib)




