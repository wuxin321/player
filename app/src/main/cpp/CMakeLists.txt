cmake_minimum_required(VERSION 3.4.1)


# 创建一个变量 source_file 它的值就是src/main/cpp/ 所有的.cpp文件
file(GLOB source_file *.cpp)


include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/libs/armeabi-v7a")

add_library(
        native-lib

        SHARED

        ${source_file})




target_link_libraries(
        native-lib
#        avfilter avformat    avcodec  avutil  swresample swscale
        avformat avcodec avfilter    avutil swresample swscale
#        avfilter
#        avformat
#        avcodec
#        avutil
#        swresample
#        swscale

        log z android  OpenSLES)