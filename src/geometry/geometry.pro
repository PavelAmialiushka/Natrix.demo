#-------------------------------------------------
#
# Project created by QtCreator 2011-04-27T19:21:58
#
#-------------------------------------------------

QT       += widgets xml gui network

TARGET = geometry
TEMPLATE = lib
CONFIG += staticlib

include( ../common.pri )

# profiler
#QMAKE_CXXFLAGS_DEBUG += -g3 -pg
#QMAKE_LFLAGS_DEBUG += -pg -lgmon

SOURCES += \
    point.cpp \
    object.cpp \
    node.cpp \
    command.cpp \
    primitives.cpp \
    scene.cpp \
    manipulator.cpp \
    line.cpp \
    worldSystem.cpp \
    joiner.cpp \
    drawer.cpp \
    manipulatorTools.cpp \
    endCupJoiner.cpp \
    bendJoiner.cpp \
    teeJoiner.cpp \
    weldJoiner.cpp \
    adorner.cpp \
    nodePointAdorner.cpp \
    toolInfo.cpp \
    manipulatorElementStart.cpp \
    manipulatorLineStart.cpp \
    manipulatorLineContinue.cpp \
    manipulatorElementRotate.cpp \
    element.cpp \
    straightLineAdorner.cpp \
    manipulatorErase.cpp \
    canvasRectangle.cpp \
    sceneProcedures.cpp \
    elementFactory.cpp \
    elementTypes.cpp \
    label.cpp \
    path.cpp \
    manipulatorLineBreaker.cpp \
    manipulatorSelector.cpp \
    document.cpp \
    manipulatorMoveContinue.cpp \
    manipulatorMoveStart.cpp \
    moveProcedures.cpp \
    moveAdorner.cpp \
    ghostAdorner.cpp \
    objectFactory.cpp \
    textLabel.cpp \
    manipulatorTextStart.cpp \
    manipulatorTextEdit.cpp \
    baseObject.cpp \
    neighbourhood.cpp \
    manipulatorMoveLabel.cpp \
    TMatrix.cpp \
    textInfo.cpp \
    graphics.cpp \
    graphicsScene.cpp \
    GraphicsTextItem.cpp \
    graphicsClipItem.cpp \
    rectangleAdorner.cpp \
    marker.cpp \
    manipulatorTextLine.cpp \
    markLabels.cpp \
    sceneProperties.cpp \
    pointProcedures.cpp \
    manipulatorWeldStart.cpp \
    manipulatorWeldMove.cpp \
    weldProcedures.cpp \
    glueObject.cpp \
    manipulatorLabelStart.cpp \
    sceneImpl.cpp \
    manipulatorCanvasMove.cpp \
    manipulatorCanvasStart.cpp \
    grip.cpp \
    textGrip.cpp \
    manipulatorTextLine2.cpp \
    canvasGrip.cpp \
    ExclusiveFile.cpp \
    DocumentAutoSaver.cpp \
    manipulatorPasteTool.cpp \
    manipulatorLabelRotate.cpp \
    SingletonFileWindow.cpp \
    scenePropertyValue.cpp \
    manipulatorLineShortener.cpp \
    moveProceduresData.cpp

HEADERS += \
    point.h \
    object.h \
    node.h \
    command.h \
    primitives.h \
    primitives.h \
    scene.h \
    manipulator.h \
    line.h \
    worldSystem.h \
    WorldSystemImpl.h \
    joiner.h \
    drawer.h \
    manipulatorTools.h \
    endCupjoiner.h \
    bendJoiner.h \
    teeJoiner.h \
    joiners.h \
    weldJoiner.h \
    adorner.h \
    nodePointAdorner.h \
    toolInfo.h \
    global.h \
    element.h \
    manipulatorElementStart.h \
    manipulatorLineStart.h \
    manipulatorLineContinue.h \
    manipulatorElementRotate.h \
    straightLineAdorner.h \
    elementFactory.h \
    elementTypes.h \
    manipulatorErase.h \
    sceneProcedures.h \
    label.h \
    path.h \
    canvasRectangle.h \
    manipulatorLineBreaker.h \
    manipulatorSelector.h \
    document.h \
    manipulatorMoveStart.h \
    manipulatorMoveContinue.h \
    moveProcedures.h \
    moveAdorner.h \
    ghostAdorner.h \
    objectFactory.h \
    textLabel.h \
    manipulatorTextStart.h \
    manipulatorTextEdit.h \
    textInfo.h \
    neighbourhood.h \
    utilites.h \
    TMatrix.h \
    manipulatorMoveLabel.h \
    graphics.h \
    dispatcher.h \
    graphicsScene.h \
    GraphicsTextItem.h \
    graphicsClipItem.h \
    rectangleAdorner.h \
    marker.h \
    manipulatorTextLine.h \
    markLabels.h \
    scales.h \
    sceneProperties.h \
    baseObject.h \
    makeSmart.h \
    pointProcedures.h \
    manipulatorWeldStart.h \
    manipulatorWeldMove.h \
    weldProcedures.h \
    glueObject.h \
    manipulatorLabelStart.h \
    sceneImpl.h \
    manipulatorCanvasStart.h \
    manipulatorCanvasMove.h \
    grip.h \
    textGrip.h \
    manipulatorTextLine2.h \
    canvasGrip.h \
    ExclusiveFile.h \
    manipulatorPasteTool.h \
    DocumentAutoSaver.h \
    lineinfo.h \
    manipulatorLabelRotate.h \
    SingletonFileWindow.h \
    scenePropertyValue.h \
    performanceLogger.h \
    manipulatorLineShortener.h \
    moveProceduresData.h
