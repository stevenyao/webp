TARGET = qwebp
include(../../qpluginbase.pri)

QMAKE_LFLAGS_RELEASE += /BASE:0x67C00000

CONFIG(debug, debug)
{
	LIBS += webplibs/libwebp_debug.lib
}

CONFIG(release, release)
{
	LIBS += webplibs/libwebp.lib
}

INCLUDEPATH = libwebp/src/
SOURCES += \
    webpioplugin.cpp \
    webphandler.cpp \
   
HEADERS += \
    webphandler.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
