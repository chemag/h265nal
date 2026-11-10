

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


# Strict Debug builds (CMakeLists.txt warning flags plus -Werror) with gcc and
# clang, followed by the tests. JOBS sets the build parallelism.
JOBS ?= 4

check-debug:
	for cc in gcc:g++ clang:clang++; do \
	  dir=build/debug-$${cc%%:*}; \
	  \rm -rf $$dir; \
	  cmake -S . -B $$dir -DCMAKE_BUILD_TYPE=Debug \
	    -DCMAKE_C_COMPILER=$${cc%%:*} -DCMAKE_CXX_COMPILER=$${cc##*:} \
	    -DBUILD_CLANG_FUZZER=OFF && \
	  cmake --build $$dir --parallel $(JOBS) && \
	  ctest --test-dir $$dir --output-on-failure || exit 1; \
	done
