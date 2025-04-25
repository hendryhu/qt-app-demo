QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    activityscreen.cpp \
    bolusscreen.cpp \
    cgmscreen.cpp \
    controliqscreen.cpp \
    data.cpp \
    graphwidget.cpp \
    homescreen.cpp \
    main.cpp \
    mainwindow.cpp \
    mypumpscreen.cpp \
    offscreen.cpp \
    popupmanager.cpp \
    popupwidget.cpp \
    profilescreen.cpp \
    pumpinfoscreen.cpp \
    settingsscreen.cpp \
    loggerscreen.cpp \
    logger.cpp \


HEADERS += \
    activityscreen.h \
    bolusscreen.h \
    cgmscreen.h \
    controliqscreen.h \
    data.h \
    graphwidget.h \
    homescreen.h \
    mainwindow.h \
    mypumpscreen.h \
    offscreen.h \
    popupmanager.h \
    popupwidget.h \
    profilescreen.h \
    pumpinfoscreen.h \
    settingsscreen.h \
    loggerscreen.h \
    logger.h

FORMS += \
    activityscreen.ui \
    activityscreen_1.ui \
    bolusscreen.ui \
    cgmscreen.ui \
    controliqscreen.ui \
    homescreen.ui \
    mainwindow.ui \
    mypumpscreen.ui \
    offscreen.ui \
    popupwidget.ui \
    settingsscreen.ui \
    loggerscreen.ui \
    profilescreen.ui \
    pumpinfoscreen.ui \
    settingsscreen.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/resources.qrc
