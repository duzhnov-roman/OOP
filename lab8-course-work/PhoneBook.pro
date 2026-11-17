QT += widgets
CONFIG += c++17
TEMPLATE = app
TARGET = PhoneBook

SOURCES += main.cpp \
           mainwindow.cpp \
           contactdialog.cpp \
           validation.cpp \
           contactstore.cpp

HEADERS += contact.h \
           mainwindow.h \
           contactdialog.h \
           validation.h \
           contactstore.h

msvc: QMAKE_CXXFLAGS += /utf-8
