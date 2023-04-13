CFLAGS = -Wall -g
LIBS = -lglfw -ldl -lm

INCLUDES = -I external
BIN = build
OBJ = memory.c font.c mesh.c shader.c graphics.c common.c image.c utils/file_handler.c
PROG = $(BIN)/p.exe


.PHONY: clean dev


$(PROG): main.c $(OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(OBJ) main.c $(LIBS)

dev:
	apt install libglfw3 libglfw3-dev

clean:
	rm $(BIN)/*
