all:
	mkdir -p bin
	gcc -Wall -Wextra -o bin/rayt src/main.c src/args.c src/tpool.c src/tga.c src/render.c -lpthread -lm
clean:
	mkdir -p bin
	rm bin/rayt