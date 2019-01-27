#-------------------------------------------------
#
# Project created by QtCreator 2011-04-28T16:42:55
#
#-------------------------------------------------

QT       += widgets core gui xml opengl printsupport network

TARGET = ui
TEMPLATE = app

include( ../common.pri )

# profiler
#QMAKE_CXXFLAGS_DEBUG += -g3 -pg
#QMAKE_LFLAGS_DEBUG += -pg -lgmon

#http://www.3dh.de/3dh.de/2006/12/19/qt-automated-unit-tests-with-qmake/
CONFIG( debug, debug|release ) {

#   cryptoPP
    #DEFINES += NOCRYPTO
    LIBS += ../cryptopp/debug/libcryptopp.a
    PRE_TARGETDEPS += ../cryptopp/debug/libcryptopp.a

    LIBS += ../geometry/debug/libgeometry.a \
            ../cryptopp/debug/libcryptopp.a

    PRE_TARGETDEPS += ../cryptopp/debug/libcryptopp.a \
                      ../geometry/debug/libgeometry.a \
                      ../secman/debug/secman.exe

    QMAKE_POST_LINK += ../secman/debug/secman --storecrc=../ui/debug/ui.exe

} else { 
    DEFINES += NDEBUG

    LIBS += ../geometry/release/libgeometry.a \
            ../cryptopp/release/libcryptopp.a

    PRE_TARGETDEPS += ../cryptopp/release/libcryptopp.a \
                      ../geometry/release/libgeometry.a \
                      ../secman/release/secman.exe

#   crc checking
    QMAKE_POST_LINK += ../secman/release/secman --storecrc=../ui/release/ui.exe
}

FORMS    += mainwindow.ui \
    exportDialog.ui \
    aboutDialog.ui \
    registerDialog.ui \
    blockDialog.ui \
    newDialog.ui \
    AutoRestoreManagerDialog.ui \
    FindDialog.ui \
    propertyTreeWidget.ui \
    CheckPointsDialog.ui
    
UI_DIRS  = ../bin/ui

TRANLATIONS = mainwindow_ru.ts

RC_FILE = mainwindow.rc

OTHER_FILES += \
    d:/work/truba.site/NextAction.txt \
    d:/work/truba/regression/testAcceptance.txt \
    mainwindow.rc \    
    classes.txt

RESOURCES += \
    mainwindow.qrc

SOURCES += main.cpp\
    mainWindow.cpp\
    WidgetBoxTreeWidget.cpp \
    sheet_delegate.cpp \
    instrumentButton.cpp \
    sceneWidget.cpp \
    sceneWidgetDrawer.cpp \
    exportDialog.cpp \
    aboutDialog.cpp \
    registerDialog.cpp \
    blockDialog.cpp \
    newDialog.cpp \
    AutoRestoreManagerDialog.cpp \
    FindDialog.cpp \
    propertyTreeWidget.cpp \
    PropertyBarModel.cpp \
    CheckPointsDialog.cpp

HEADERS  += mainwindow.h \
    WidgetBoxTreeWidget.h \
    sheet_delegate_p.h \
    instrumentbutton.h \
    sceneWidget.h \
    sceneWidgetDrawer.h \
    version.h \
    exportDialog.h \
    aboutDialog.h \
    registerDialog.h \
    blockDialog.h \
    newDialog.h \
    AutoRestoreManagerDialog.h \
    FindDialog.h \
    propertyTreeWidget.h \
    PropertyBarModel.h \
    CheckPointsDialog.h
