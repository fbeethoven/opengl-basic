CFLAGS = -Wall -g
LIBS = -lglfw -ldl

INCLUDES = -I external
BIN = build
PROG = $(BIN)/win


.PHONY: clean dev


$(PROG): main.c shader.c
	gcc -o $@ $(CFLAGS) $(INCLUDES) shader.c main.c $(LIBS)

dev:
	apt install libglfw3 libglfw3-dev

clean:
	rm $(BIN)/*
