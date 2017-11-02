cmake_minimum_required(VERSION 3.1)

project(HoneyBee)

include_directories(X11)
#include_directories(Imx6)

file(GLOB SOURCES
        ./Main.cpp
        ./HBGlobal.hpp
        ./HBContext.hpp
        ./HBShader.hpp
        ./HBShader.cpp
        ./HBEgl.hpp
        ./HBEgl.cpp
        ./X11/EglAdapter.hpp
        ./X11/EglAdapter.cpp
#        ./Imx6/EglAdapter.hpp
#        ./Imx6/EglAdapter.cpp
    )
add_executable(${PROJECT_NAME} ${SOURCES})

SET(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(X11)
#find_library(M_LIB m)
find_library( OPENGLES3_LIBRARY GLESv2 "OpenGL ES v3.0 library")
find_library( EGL_LIBRARY EGL "EGL 1.4 library" )
target_link_libraries(${PROJECT_NAME} ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} ${X11_LIBRARIES})
