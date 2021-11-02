emulator:
	gcc emulator.c `sdl2-config --cflags --libs` -o emulator -lm

run:
	./emulator