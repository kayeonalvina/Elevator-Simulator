QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audioSystem.cpp \
    buildingSafetyService.cpp \
    button.cpp \
    display.cpp \
    elevator.cpp \
    elevatorControlSystem.cpp \
    floor.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    audioSystem.h \
    buildingSafetyService.h \
    button.h \
    defs.h \
    display.h \
    elevator.h \
    elevatorControlSystem.h \
    floor.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
