#-------------------------------------------------
#
# Project created by QtCreator 2013-12-04T17:21:33
#
#-------------------------------------------------

QT       += core gui network

LIBS += -lportaudio -lopus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CamelBabel
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        MainWindow.cpp \
    SettingsDialog.cpp \
    ChatWidget.cpp \
    RTPCallManager.cpp \
    ConnectionDialog.cpp \
    Opus.cpp

HEADERS  += MainWindow.hpp \
    SettingsDialog.hpp \
    ChatWidget.hpp \
    RTPCallManager.hpp \
    rtp.hpp \
    ConnectionDialog.hpp \
    BabelException.hpp \
    PortAudio.hpp \
    Opus.hpp

FORMS    += MainWindow.ui \
    SettingsDialog.ui \
    ChatWidget.ui

RESOURCES += \
    CamelBabel.qrc
