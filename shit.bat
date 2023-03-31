set CC = gcc
set CFLAGS = -Wall -g
set LIBS = -lglfw -ldl -lm

set INCLUDES = -I
set BIN = build
set OBJ = shader.c graphics.c common.c image.c utils/file_handler.c
set PROG = %BIN%


set .PHONY: clean dev


set %PROG%: %main.c% %OBJ%
	gcc -o build/p.exe %PROG% %CFLAGS% %INCLUDES% %OBJ% %main.c% %LIBS%

set dev:
	apt install libglfw3 libglfw3-dev

set clean:
	rm %BIN%/*