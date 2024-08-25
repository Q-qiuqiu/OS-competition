cmake_minimum_required(VERSION 3.17)
project(video_decoder)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
        libavformat
        libavcodec
        libswscale
        libavutil
)
find_package(OpenCV REQUIRED)

add_executable(video_decoder main.cpp video_decoder.cpp infer.cpp)

target_link_libraries(video_decoder PRIVATE PkgConfig::LIBAV ${OpenCV_LIBS})

install(TARGETS video_decoder DESTINATION bin)
