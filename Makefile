CMAKE_FLAGS = -DCMAKE_BUILD_TYPE=Debug
ifdef LLVM_DIR
    CMAKE_FLAGS += -DLLVM_DIR=$(LLVM_DIR)
endif

all: 
	cmake --build build
run: all
	./build/kaleidoscope
run-debug: all
	lldb ./build/kaleidoscope
clean:
	rm -rf build
	cmake -S . -B build $(CMAKE_FLAGS)
