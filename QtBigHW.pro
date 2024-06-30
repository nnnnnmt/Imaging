######################################################################
# Automatically generated by qmake (3.1) Tue May 7 16:22:41 2024
######################################################################


TEMPLATE = app
TARGET = QtBigHW
INCLUDEPATH += .
QT += widgets
QT += network
# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += \
            C:\Qt\OpenCV\opencv\opencv-build\install\include

LIBS += C:\Qt\OpenCV\opencv\opencv-build\lib/libopencv_*

# Input
HEADERS += connectline.h \
           editablelabel.h \
           editnode.h \
           gradientheader.h \
           mainscene.h \
           mainview.h \
           mainwindow.h \
           node.h \
           nodechoice.h \
           terminal.h \
           inputnode.h \
           outputnode.h \
           functionnode.h \
           addnode.h \
           imagewidget.h \
           ainode.h \
           utils.h
FORMS += mainwindow.ui \
    addnodewidget.ui \
    aii2inodewidget.ui \
    aiw2inodewidget.ui \
    canvaswindow.ui \
    cropwindow.ui \
    editcanvaswindow.ui \
    editcropwindow.ui \
    editnodewidget.ui \
    editwindow.ui \
    filternodewidget.ui \
    functionnodewidget.ui \
    inputnodewidget.ui \
    nodechoice.ui \
    outputnodewidget.ui \
    preprocessingwindow.ui
SOURCES += connectline.cpp \
           editablelabel.cpp \
           editnode.cpp \
           gradientheader.cpp \
           main.cpp \
           mainscene.cpp \
           mainview.cpp \
           mainwindow.cpp \
           node.cpp \
           nodechoice.cpp \
           terminal.cpp \
           inputnode.cpp \
           outputnode.cpp \
           functionnode.cpp \
           addnode.cpp \
           imagewidget.cpp \
           ainode.cpp \
           utils.cpp

RESOURCES += \
    nodechoiceresources.qrc \
    nodewidgetresources.qrc \
    windowresources.qrc
