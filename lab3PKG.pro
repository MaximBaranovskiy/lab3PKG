QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += C:\Users\maxim\Downloads\opencv\release\install\include

LIBS += C:\Users\maxim\Downloads\opencv\release\bin\libopencv_core480.dll
LIBS += C:\Users\maxim\Downloads\opencv\release\bin\libopencv_imgproc480.dll
LIBS += C:\Users\maxim\Downloads\opencv\release\bin\libopencv_imgcodecs480.dll
LIBS += C:\Users\maxim\Downloads\opencv\release\bin\libopencv_videoio480.dll
LIBS += C:\Users\maxim\Downloads\opencv\release\bin\libopencv_highgui480.dll
LIBS += C:\Users\maxim\Downloads\opencv\release\bin\libopencv_flann480.dll
LIBS += C:\Users\maxim\Downloads\opencv\release\bin\libopencv_ml480.dll
LIBS += C:\Users\maxim\Downloads\opencv\release\bin\libopencv_photo480.dll
LIBS += C:\Users\maxim\Downloads\opencv\release\bin\libopencv_dnn480.dll

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES +=
