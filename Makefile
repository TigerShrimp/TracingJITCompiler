all: build/Makefile
	@cd build && make

build/Makefile: | build
	@cd build && cmake ..

build:
	@mkdir build

clean:
	@cd build && make clean

clean_all:
	@rm -rf build