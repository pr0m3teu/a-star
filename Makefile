
main: main.c
	clang -o main -Wall -Wextra -std=c11 -lm -I./raylib main.c ./raylib/libraylib.a

a-star: a-star.c
	clang -o a-star -Wall -Wextra -std=c11 a-star.c
