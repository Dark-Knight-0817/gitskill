cmake_minimum_required(VERSION 3.5.0)
project(my_test VERSION 0.1.0 LANGUAGES C CXX)

#找到FFmpeg包
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libavdevice
    libswresample
    libpostproc
    libswscale
)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

#添加头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/inc ${FFMPEG_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

#设置源代码目录变量
set(src_dir ${PROJECT_SOURCE_DIR}/src)
# 明确列出所有源文件
set(SOURCES
    ${src_dir}/main.cpp
)

add_executable(my_test ${SOURCES})

# 设置 C++ 标准
set_target_properties(my_test PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

#链接FFmpeg库和SDL库
target_link_libraries(my_test
    PRIVATE
    PkgConfig::FFMPEG
    PkgConfig::SDL2
)