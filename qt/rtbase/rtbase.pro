SOURCES += \
    ../../src/texture.cpp \
    ../../src/scene.cpp \
    ../../src/material.cpp \
    ../../src/main.cpp \
    ../../src/log.cpp \
    ../../src/geom.cpp

HEADERS += \
    ../../include/texture.h \
    ../../include/scene.h \
    ../../include/material.h \
    ../../include/main.h \
    ../../include/log.h \
    ../../include/geom.h \
    ../../include/basic_structs.h

INCLUDEPATH += \
    ../../include/

CONFIG += c++11

QMAKE_CFLAGS += -std=c99

contains(QT_VERSION, ^4.*) {
QMAKE_CXXFLAGS += -std=gnu++0x
}
