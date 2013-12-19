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
    ChatWidget.cpp \
    RTPCallManager.cpp \
    Opus.cpp \
    SipHandler.cpp \
    SettingsDialog.cpp

HEADERS  += MainWindow.hpp \
    ChatWidget.hpp \
    RTPCallManager.hpp \
    rtp.hpp \
    BabelException.hpp \
    PortAudio.hpp \
    IAudioAPI.hpp \
    IEncoder.hpp \
    Opus.hpp \
    RTPPacket.hpp \
    SipProtocol.hpp \
    SipHandler.hpp \
    SettingsDialog.hpp

FORMS    += MainWindow.ui \
    ChatWidget.ui \
    SettingsDialog.ui

RESOURCES += \
    CamelBabel.qrc
