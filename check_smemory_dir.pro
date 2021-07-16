CONFIG += c++98
CONFIG += console
CONFIG -= app_bundle
CONFIG += boost164

SOURCES += \
    Main.cpp

HEADERS += \
    CommandLineArgs.h \
    Cons.h

LIBS += -lboost_system
LIBS += -lboost_filesystem
LIBS += -lboost_program_options
