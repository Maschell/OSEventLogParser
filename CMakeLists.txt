cmake_minimum_required(VERSION 3.18)
project(oseventlog_parser)

set(CMAKE_CXX_STANDARD 20)

add_executable(oseventlog_parser
        main.cpp
        wut_structsize.h
        OSLogRetrieve.h
        OSEventLog.h
        OSEventLog.cpp
        StringTools.h
        stdendian.h)
