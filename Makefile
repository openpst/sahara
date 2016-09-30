default: qmake

qmake:	
	SDIR=$(PWD)
	if [ ! -d "./lib/libopenpst/include" ]; then git submodule init && git submodule update;  fi
	if [ ! -d "./lib/libopenpst/lib/serial/include" ]; then cd ./lib/libopenpst/ && git submodule init && git submodule update;  fi
	qmake -makefile -o ./build/Makefile sahara.pro 
	$(MAKE) -C build