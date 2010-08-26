TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . ../../src/
CONFIG += qtestlib
DEFINES += WORKER_TEST

# Input

SOURCES += testrecord.cpp ../../src/record.cpp ../../src/constants.cpp

HEADERS += ../../src/record.h ../../src/constants.h
