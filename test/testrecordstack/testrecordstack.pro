TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . ../../src/
CONFIG += qtestlib
DEFINES += WORKER_TEST

# Input

SOURCES += testrecordstack.cpp ../../src/recordstack.cpp ../../src/constants.cpp ../../src/record.cpp

HEADERS += ../../src/record.h ../../src/constants.h ../../src/recordstack.h
