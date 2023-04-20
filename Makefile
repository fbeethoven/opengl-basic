CFLAGS = -Wall -g
LIBS = -lglfw -ldl -lm

INCLUDES = -I ../external
BIN = ../build
OBJ = memory.c font.c mesh.c shader.c graphics.c common.c image.c \
	  utils/helpers.c utils/file_handler.c
PROG = $(BIN)/p


.PHONY: clean dev all

all: clean $(PROG)
	$(PROG)

$(PROG): main.c $(OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(OBJ) main.c $(LIBS)

dev:
	apt install libglfw3 libglfw3-dev

clean:
	rm $(BIN)/*
