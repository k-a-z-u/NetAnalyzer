# Usage:
#  Create build folder, like NetAnalyzer-build next to NetAnalyzer folder
#  CD into build folder and execute 'cmake -DCMAKE_BUILD_TYPE=Debug ../NetAnalyzer'
#  make

# When gcc-4.7+ is not the systems default compiler (like on the RaspberryPi) use:
#  CC=/usr/bin/gcc-4.7 CXX=/usr/bin/g++-4.7 cmake -DCMAKE_BUILD_TYPE=Release ../NetAnalyzer

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(NetAnalyzer)

IF (NOT CMAKE_BUILD_TYPE)
	MESSAGE(STATUS "No build type selected. Default to Debug")
	SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

# build-type info
MESSAGE(STATUS "build-type:		" ${CMAKE_BUILD_TYPE})

# todo: make optional?
# currently always required: libpcap
#FIND_PACKAGE(PCAP REQUIRED)
ADD_DEFINITIONS(-DWITH_PCAP)

# always required: ncurses
SET(EXTRA_LIBS ${EXTRA_LIBS} ncurses)


OPTION(WITH_ZLIB "enable gzip (de)compression support. also allows decompressing compressed HTTP streams." OFF)
OPTION(WITH_LZMA "enabled lzma (de)compression support." OFF)
OPTION(WITH_LZO "enable lzo (de)compression support." OFF)
OPTION(WITH_LZ4 "enable lz4 (de)compression support." OFF)
OPTION(WITH_QUICKLZ "enable quickLZ (de)compression support." OFF)

# add gzip (de)compression support?
IF(WITH_ZLIB)
	FIND_PACKAGE(ZLIB REQUIRED)
	ADD_DEFINITIONS(-DWITH_ZLIB)
	MESSAGE(STATUS "using ZLIB		yes")
ELSE()
	MESSAGE(STATUS "using ZLIB		NO")
ENDIF()

# add lzma (de)compression support?
IF(WITH_LZMA)
	ADD_DEFINITIONS(-DWITH_LZMA)
	FIND_PACKAGE(LibLZMA REQUIRED)
	INCLUDE_DIRECTORIES(${LIBLZMA_INCLUDE_DIRS})
	SET(EXTRA_LIBS ${EXTRA_LIBS} ${LIBLZMA_LIBRARIES})
	MESSAGE(STATUS "using LZMA:		yes")
ELSE()
	MESSAGE(STATUS "using LZMA:		NO")
ENDIF()

# add lzo (de)compression support?
if(WITH_LZO)
	ADD_DEFINITIONS(-DWITH_LZO)
	SET(EXTRA_LIBS ${EXTRA_LIBS} lzo2)
	MESSAGE(STATUS "using LZO		yes")
ELSE()
	MESSAGE(STATUS "using LZO		NO")
ENDIF()

# add lz4 (de)compression support?
if(WITH_LZ4)
	ADD_DEFINITIONS(-DWITH_LZ4)
	MESSAGE(STATUS "using LZ4		yes")
ELSE()
	MESSAGE(STATUS "using LZ4		NO")
ENDIF()

# add quickLZ (de)compression support?
IF (WITH_QUICKLZ)
	ADD_DEFINITIONS(-DWITH_QUICKLZ)
	MESSAGE(STATUS "using QuickLZ	yes")
ELSE()
	MESSAGE(STATUS "using QuickLZ	NO")
ENDIF()

#OPTION(WITH_LZMA "Try to compile with support for LZMA compression statistics" ON)
# TODO: Need FIND_PACKAGE, INCLUDE_DIRECTORIES, TARGET_LINK_LIBRARIES for: pcap, ncurses, pthread, lzo

#INCLUDE_DIRECTORIES(
#   ${ZLIB_INCLUDE_DIRS}
#)

FILE(GLOB NETANALYZER_HEADERS	src/*.h
								src/*/*.h
								src/*/*/*.h
								src/*/*/*/*.h)

FILE(GLOB NETANALYZER_SOURCES	src/*.cpp
								src/*/*.cpp
								src/*/*/*.cpp
								src/*/*/*/*.cpp
								src/*.c
								src/*/*.c
								src/*/*/*.c
								src/*/*/*/*.c)

ADD_DEFINITIONS(
	-Wall
	-Wextra
#	-pedantic
	-std=c++11
	-D_FILE_OFFSET_BITS=64
)

SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG")

ADD_EXECUTABLE(${PROJECT_NAME} ${NETANALYZER_HEADERS} ${NETANALYZER_SOURCES} ${NETANALYZER_TXT})

# TODO: write a FindPcap script to select the correct lib depending on the platform.
IF (CYGWIN)
	SET(PCAP_LIBRARY_NAME "wpcap")
ELSE()
	SET(PCAP_LIBRARY_NAME "pcap")
ENDIF()

TARGET_LINK_LIBRARIES(
   ${PROJECT_NAME}
   ${ZLIB_LIBRARIES}
   ${PCAP_LIBRARY_NAME}
   pthread
   ${EXTRA_LIBS}
)
