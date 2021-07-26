TEMPLATE = app
CONFIG -= qt

 CONFIG += c++98
 CONFIG += console
 CONFIG += boost164
CONFIG -= app_bundle

#DEFINES += NDIAG

INCLUDEPATH += $${PWD}/include
INCLUDEPATH += .

unix{
    QMAKE_LFLAGS   += -Wl,--no-undefined -Wl,--trace
    LIBS += -ldl
}
win32{
    LIBS += -lWs2_32
    DEFINES += WIN32 NOMINMAX _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS BOOST_NO_CXX11_VARIADIC_TEMPLATES _WINSOCK_DEPRECATED_NO_WARNINGS
    DEFINES -= UNICODE
    QMAKE_CFLAGS   -= -Zc:wchar_t-
    QMAKE_CFLAGS   += -Zc:wchar_t
    QMAKE_CXXFLAGS -= -Zc:wchar_t-
    QMAKE_CXXFLAGS += -Zc:wchar_t
    QMAKE_LIBDIR   += $$(NITALINK)
}

CONFIG(debug, debug|release) {
    DEFINES    += _DEBUG
    DEFINES    -= NDEBUG
    }

CONFIG(release, debug|release){
    DEFINES    -= _DEBUG
    DEFINES    += NDEBUG
    }

EXT = ""
linux-g++*:{EXT = ".bin"}
TARGET      = $${TARGET}$${EXT}


SOURCES += \
    Main.cpp

HEADERS += \
    CommandLineArgs.h \
    Cons.h

LIBS += -lreg_12

unix{
    LIBS += -lboost_system
    LIBS += -lboost_filesystem
    LIBS += -lboost_program_options
    LIBS += -lpthread
}
