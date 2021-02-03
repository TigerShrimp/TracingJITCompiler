all: build/Makefile
	@cmake --build build --parallel

build/Makefile: | build
	@cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..

build:
	@mkdir build

clean:
	@cd build && make clean

clean_all:
	@rm -rf build