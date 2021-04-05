cmake_minimum_required(VERSION 3.0)
project(qrcode)
set(CMAKE_CXX_FLAGS -std=c++11)
file(GLOB src_files
        src/*.cpp
        src/*.c
)
find_package(OpenCV)
add_executable(qrcode test/QRcode_test.cpp ${src_files})
target_link_libraries(qrcode opencv_core opencv_imgproc opencv_highgui)
