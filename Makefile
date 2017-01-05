default: qmake

qmake:	
	if [ ! -d "./build" ]; then mkdir build; fi
	if [ ! -d "./lib/libopenpst/include" ]; then git submodule init && git submodule update;  fi
	if [ ! -d "./lib/libopenpst/lib/serial/include" ]; then cd ./lib/libopenpst/ && git submodule init && git submodule update;  fi
	qmake -makefile -o ./build/Makefile sahara.pro 
	$(MAKE) -C build

clean:
	rm -rf build/*
	rm -rf lib/libopenpst/build/*