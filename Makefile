all: 
	cmake --build build
run: all
	./build/kaleidoscope
clean:
	rm -rf build
	cmake -S . -B build
