

all: none
	echo "use cmake to build"


format:
	clang-format -i -style=google ./include/*h
	clang-format -i -style=google ./src/*cc
	clang-format -i -style=google ./tools/*cc


.PHONY: build

build:
	\rm -rf build
	mkdir build
	cd build && CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=DEBUG ..
	cd build && make -j 8

