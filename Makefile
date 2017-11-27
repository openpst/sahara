####
## Makefile for openpst/sahara
####
QMAKE = qmake

all: default

default: check debug release

debug: check _debug

release: check _release

check:
	if [ ! -d "./build" ]; then mkdir -p build;  fi
	if [ ! -d "./lib/libopenpst/include" ] || [ ! -d "./lib/gui-common/include" ]; then git submodule init && git submodule update;  fi
	if [ ! -d "./lib/libopenpst/lib/serial/include" ]; then cd ./lib/libopenpst/ && git submodule init && git submodule update;  fi

_debug:
	$(QMAKE) -makefile -o ./build/Makefile sahara.pro "CONFIG+=debug" "CONFIG-=release"
	$(MAKE) -C build

_release:
	$(QMAKE) -makefile -o ./build/Makefile sahara.pro "CONFIG-=debug" "CONFIG+=release"
	$(MAKE) -C build

clean:
	rm -rf build/*
	rm -rf lib/libopenpst/build/*
