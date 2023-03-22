CFLAGS = -Wall -g
LIBS = -lglfw -ldl

INCLUDES = -I external
BIN = build
OBJ = shader.c graphics.c
PROG = $(BIN)/win


.PHONY: clean dev


$(PROG): main.c $(OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(OBJ) main.c $(LIBS)

dev:
	apt install libglfw3 libglfw3-dev

clean:
	rm $(BIN)/*
