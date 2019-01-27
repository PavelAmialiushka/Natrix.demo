#-------------------------------------------------
#
# Project created by QtCreator 2011-04-27T19:26:24
#
#-------------------------------------------------

QT       += core gui xml opengl printsupport

TARGET = tst_geometry
TEMPLATE = app
CONFIG   += console

include( ../common.pri )

# profiler
#QMAKE_CXXFLAGS_DEBUG += -g3 -pg
#QMAKE_LFLAGS_DEBUG += -pg -lgmon

CONFIG( debug, debug|release ) {
    LIBS += ../geometry/debug/libgeometry.a \
            ../cryptopp/debug/libcryptopp.a
    PRE_TARGETDEPS += ../geometry/debug/libgeometry.a
    QMAKE_POST_LINK+= ../geometrytest/debug/tst_geometry.exe
} else {
    DEFINES += NDEBUG
    LIBS += ../geometry/release/libgeometry.a \
            ../cryptopp/release/libcryptopp.a
    PRE_TARGETDEPS += ../geometry/release/libgeometry.a
    QMAKE_POST_LINK+= ../geometrytest/release/tst_geometry.exe
}

SOURCES += \
    ../gtest/src/gtest-all.cc \
    main.cpp \
    test_worldsystem.cpp \
    test_scene.cpp \
    test_point.cpp \
    test_tmatrix.cpp \
    test_elements.cpp \
    test_graphics.cpp \
    test_manipulator.cpp

HEADERS += \
    ../gtest/gtest-typed-test.h \
    ../gtest/gtest-test-part.h \
    ../gtest/gtest-spi.h \
    ../gtest/gtest-printers.h \
    ../gtest/gtest-param-test.h.pump \
    ../gtest/gtest-param-test.h \
    ../gtest/gtest-message.h \
    ../gtest/gtest-death-test.h \
    ../gtest/gtest_prod.h \
    ../gtest/gtest_pred_impl.h \
    ../gtest/gtest.h \
    ../gtest/src/gtest-typed-test.cc \
    ../gtest/src/gtest-test-part.cc \
    ../gtest/src/gtest-printers.cc \
    ../gtest/src/gtest-port.cc \
    ../gtest/src/gtest-filepath.cc \
    ../gtest/src/gtest-death-test.cc \
    ../gtest/src/gtest_main.cc \
    ../gtest/src/gtest.cc \
    test_command.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"
