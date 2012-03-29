
TEMPLATE = app
TARGET = hull

CONFIG += QtGui

CONFIG -= debug
CONFIG += release
QT += opengl

OBJECTS_DIR = bin

QMAKE_CXXFLAGS += -std=c++0x -Wall -Wextra -pedantic-errors -g -DNDEBUG

DEPENDPATH += src \
              include \
              include/common \
              include/io \
              include/visualization \
              include/geom/primitives \
              include/geom/hull \
              include/treap \
              include/window
              
INCLUDEPATH += . \
              include 

# Input

SOURCES += main.cpp treap_node.cpp hull.cpp hull_checks.cpp

LIBS += -Llib -lvisualization
