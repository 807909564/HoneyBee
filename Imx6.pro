TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += Platform/Imx6

SOURCES += Main_Imx6.cpp \
    HBEgl.cpp \
    HBHelper.cpp \
    HBMesh.cpp \
    HBModel.cpp \
    Platform/Imx6/EglAdapter.cpp

HEADERS += \
    HBContext.hpp \
    HBEgl.hpp \
    HBGlobal.hpp \
    HBHelper.hpp \
    HBMesh.hpp \
    HBModel.hpp \
    Platform/Imx6/EglAdapter.hpp

LIBS += -lassimp -lpng -lEGL -lm -lGLESv2 -lz
