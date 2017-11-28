cmake_minimum_required(VERSION 3.1)

project(HoneyBee)

INCLUDE_DIRECTORIES(X11)
#INCLUDE_DIRECTORIES(Imx6)
INCLUDE_DIRECTORIES(../assimp/include)
LINK_DIRECTORIES(../assimp/lib)

INCLUDE_DIRECTORIES(../freetype/include)
LINK_DIRECTORIES(../freetype/lib)

file(GLOB SOURCES
        ./Main.cpp
        ./HBGlobal.hpp
        ./HBContext.hpp
        ./HBHelper.hpp
        ./HBHelper.cpp
        ./HBEgl.hpp
        ./HBEgl.cpp
        ./HBMesh.hpp
        ./HBMesh.cpp
        ./HBModel.hpp
        ./HBModel.cpp
        ./HBText.hpp
        ./HBText.cpp
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
find_library(OPENGLES3_LIBRARY GLESv2 "OpenGL ES v3.0 library")
find_library(EGL_LIBRARY EGL "EGL 1.4 library" )

target_link_libraries(${PROJECT_NAME} ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} ${X11_LIBRARIES} assimp freetype png)
