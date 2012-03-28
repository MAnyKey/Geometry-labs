
TEMPLATE = app
TARGET = hull

CONFIG += QtGui debug
CONFIG -= release
QT += opengl

OBJECTS_DIR = bin

QMAKE_CXXFLAGS += -std=c++0x -Wall -Wextra -pedantic-errors -g -pg -O0

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

SOURCES += main.cpp treap_node.cpp hull.cpp

LIBS += -Llib -lvisualization
