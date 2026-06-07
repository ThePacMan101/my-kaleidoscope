CMAKE_FLAGS =
ifdef LLVM_DIR
    CMAKE_FLAGS += -DLLVM_DIR=$(LLVM_DIR)
endif

all: 
	cmake --build build
run: all
	./build/kaleidoscope
clean:
	rm -rf build
	cmake -S . -B build $(CMAKE_FLAGS)
