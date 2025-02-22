#!/bin/bash
rm -rf build
mkdir build
cd build
cmake ..
make
./my_test "/Users/darkknight/Documents/learning resource/backups/movie/1.mp4" hardware_video.yuv

# 直接播放 YUV
ffplay -f rawvideo -pixel_format yuv420p -video_size 800x720 hardware_video.yuv