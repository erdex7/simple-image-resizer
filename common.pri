ROOT_PATH = $${PWD}

CONFIG(debug, debug|release) {
	BUILD_FLAG = debug
	LIB_SUFFIX = d
} 
else {
	BUILD_FLAG = release
}

BUILD_PATH = $${ROOT_PATH}/build/$${BUILD_FLAG}

BIN_PATH = $${ROOT_PATH}/bin/$${BUILD_FLAG}
DESTDIR = $${BIN_PATH}

QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"
