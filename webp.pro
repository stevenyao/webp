TARGET = qwebp
include(../../qpluginbase.pri)

QMAKE_LFLAGS_RELEASE += /BASE:0x67C00000

WEBP_PATH = libwebp/src/

INCLUDEPATH = libwebp/src/
SOURCES += \
    webpioplugin.cpp \
    webphandler.cpp \
    $$files($$WEBP_PATH/dec/*.c)\
    $$files($$WEBP_PATH/utils/*.c) \
    $$files($$WEBP_PATH/dsp/*.c)
   
HEADERS += \
    webphandler.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
