all: build/Makefile java
	@cmake --build build --parallel

build/Makefile: | build
	@cd bueild && cmake -DCMAKE_BUILD_TYPE=Debug ..

build:
	@mkdir build

java:
	@javac test/*.java

clean:
	cd build && make clean

clean_all:
	@rm -rf build

run_test: java
	@python3 test/test.py