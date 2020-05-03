CONFIG += c++11

TEMPLATE = subdirs

SUBDIRS = resizer console gui

resizer.file = src/lib/resizer.pro
console.subdir = src/console
gui.subdir = src/gui

console.depends = resizer
gui.depends = resizer

unix:QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"
