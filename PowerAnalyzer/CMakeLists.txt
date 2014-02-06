# Usage:
#  Create build folder, like PowerAnalyzer-build next to PowerAnalyzer folder
#  CD into build folder and execute 'cmake -DCMAKE_BUILD_TYPE=Debug ../PowerAnalyzer'
#  make

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(PowerAnalyzer)

IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE(STATUS "No build type selected. Default to Debug")
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

ADD_DEFINITIONS(-DWITH_ZLIB)
ADD_DEFINITIONS(-DWITH_LZMA)
ADD_DEFINITIONS(-DWITH_LZO)
ADD_DEFINITIONS(-DWITH_LZ4)
ADD_DEFINITIONS(-DWITH_QUICKLZ)

FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(LibLZMA REQUIRED)

INCLUDE_DIRECTORIES(
    ${ZLIB_INCLUDE_DIRS}
    ${LIBLZMA_INCLUDE_DIRS}
)

FILE(GLOB NETANALYZER_HEADERS
    ../NetAnalyzer/src/compression/Data.h
    ../NetAnalyzer/src/compression/Compressor.h
    ../NetAnalyzer/src/compression/Decompressor.h
    ../NetAnalyzer/src/compression/GZipCompress.h
    ../NetAnalyzer/src/compression/GZipDecompress.h
    ../NetAnalyzer/src/compression/QuickLZCompress.h
    ../NetAnalyzer/src/compression/QuickLZDecompress.h
    ../NetAnalyzer/src/compression/LZMACompress.h
    ../NetAnalyzer/src/compression/LZMADecompress.h
    ../NetAnalyzer/src/compression/LZOCompress.h
    ../NetAnalyzer/src/compression/LZODecompress.h
    ../NetAnalyzer/src/compression/LZ4Compress.h
    ../NetAnalyzer/src/compression/LZ4Decompress.h
    ../NetAnalyzer/src/lib/QuickLZ/quicklz.h
    ../NetAnalyzer/src/lib/LZ4/lz4.h
    ../NetAnalyzer/src/lib/LZ4/lz4_decoder.h
    ../NetAnalyzer/src/lib/LZ4/lz4_encoder.h
    ../NetAnalyzer/src/misc/MyString.h
    ../NetAnalyzer/src/misc/Scheduler.h
    ../NetAnalyzer/src/misc/Thread.h
    ../NetAnalyzer/src/Helper.h
    ../NetAnalyzer/src/Debug.h
)

FILE(GLOB NETANALYZER_SOURCES
    ../NetAnalyzer/src/compression/Data.cpp
    ../NetAnalyzer/src/compression/GZipCompress.cpp
    ../NetAnalyzer/src/compression/GZipDecompress.cpp
    ../NetAnalyzer/src/compression/QuickLZCompress.cpp
    ../NetAnalyzer/src/compression/QuickLZDecompress.cpp
    ../NetAnalyzer/src/compression/LZMACompress.cpp
    ../NetAnalyzer/src/compression/LZMADecompress.cpp
    ../NetAnalyzer/src/compression/LZOCompress.cpp
    ../NetAnalyzer/src/compression/LZODecompress.cpp
    ../NetAnalyzer/src/compression/LZ4Compress.cpp
    ../NetAnalyzer/src/compression/LZ4Decompress.cpp
    ../NetAnalyzer/src/lib/QuickLZ/quicklz.c
    ../NetAnalyzer/src/lib/LZ4/lz4.c
    ../NetAnalyzer/src/misc/MyString.cpp
    ../NetAnalyzer/src/misc/Scheduler.cpp
    ../NetAnalyzer/src/misc/Thread.cpp
    ../NetAnalyzer/src/Helper.cpp
    ../NetAnalyzer/src/Debug.cpp
)

FILE(GLOB POWERANALYZER_HEADERS
    src/common/Socket.h
    src/common/Utils.h
    src/client/ClientApp.h
    src/client/ClientSocket.h
    src/client/ClientStats.h
    src/client/ClientStatsExporter.h
    src/client/PayloadMetaDirectory.h
    src/server/ServerApp.h
    src/server/ServerSocket.h
    src/server/ServerStats.h
    src/server/ServerStatsExporter.h
    src/server/PayloadDirectory.h
    src/protocol/FileRequestInfo.h
    src/protocol/FileReceiveInfo.h
    src/PowerAnalyzerApp.h
    src/Options.h
)

FILE(GLOB POWERANALYZER_SOURCES
    src/common/Socket.cpp
    src/common/Utils.cpp
    src/client/ClientApp.cpp
    src/client/ClientSocket.cpp
    src/client/ClientStats.cpp
    src/client/ClientStatsExporter.cpp
    src/client/PayloadMetaDirectory.cpp
    src/server/ServerApp.cpp
    src/server/ServerSocket.cpp
    src/server/ServerStats.cpp
    src/server/ServerStatsExporter.cpp
    src/server/PayloadDirectory.cpp
    src/protocol/FileRequestInfo.cpp
    src/protocol/FileReceiveInfo.cpp
    src/PowerAnalyzerApp.cpp
    src/Options.cpp
    src/main.cpp
)

FILE(GLOB POWERANALYZER_EXTRA
    README
)

ADD_DEFINITIONS(
   -Wall
   -Wextra
#   -pedantic
   -D_FILE_OFFSET_BITS=64
)

SET(CMAKE_CXX_FLAGS "-std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DWITH_DEBUG")

ADD_EXECUTABLE(
    ${PROJECT_NAME}
    ${NETANALYZER_HEADERS}
    ${NETANALYZER_SOURCES}
    ${POWERANALYZER_HEADERS}
    ${POWERANALYZER_SOURCES}
    ${POWERANALYZER_EXTRA}
)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    ${ZLIB_LIBRARIES}
    ${LIBLZMA_LIBRARIES}
    lzo2
    pthread
)
