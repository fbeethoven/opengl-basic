CFLAGS = -Wall -g
LIBS = -lglfw -ldl -lm

INCLUDES = -I external
BIN = build
OBJ = shader.c graphics.c common.c image.c
PROG = $(BIN)/p.exe


.PHONY: clean dev


$(PROG): main.c $(OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(OBJ) main.c $(LIBS)

dev:
	apt install libglfw3 libglfw3-dev

clean:
	rm $(BIN)/*
