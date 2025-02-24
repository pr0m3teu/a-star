CFLAGS = -Wall -Wextra -std=c11 -O3
SDL_LIBS=-L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL2

main: main.c
	clang -o main $(CFLAGS) -lm -I./raylib main.c ./raylib/libraylib.a

a-star: a-star.c
	clang -o a-star $(CFLAGS) a-star.c

.PHONY : SDL
SDL: a-star-sdl.c
	clang -o main $(CFLAGS) $(SDL_LIBS) a-star-sdl.c
