QT += core
QT -= gui

QMAKE_CXXFLAGS += -std=c++0x
CONFIG += console
CONFIG -= app_bundle
CONFIG += boost164

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/boost
LIBS += -L/usr/include/boost -lboost_system
LIBS += -I/usr/include/boost -lboost_system
LIBS += -L/usr/include/boost  -lboost_filesystem
LIBS += -I/usr/include/boost  -lboost_filesystem
LIBS += -L/usr/include/boost  -lboost_program_options
LIBS += -I/usr/include/boost  -lboost_program_options
LIBS += -L/usr/include/boost -lboost_locale
LIBS += -I/usr/include/boost -lboost_locale

SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    CONS.h \
    CommandLineArgs.h
