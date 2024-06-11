.PHONY = all clean

all: example

example:
	emcc \
		-sINITIAL_MEMORY=67108864 \
		-sALLOW_BLOCKING_ON_MAIN_THREAD \
		-sASYNCIFY=1 \
		-sFETCH \
		-sNO_EXIT_RUNTIME=1 \
		-sALLOW_MEMORY_GROWTH=1 \
		-sEXPORTED_FUNCTIONS=['_run'] \
		-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap'] \
		src/main.cc \
		src/scene.cc \
		-Iinclude \
		-o example.js

clean:
	rm -rf *.o example.wasm example.js
