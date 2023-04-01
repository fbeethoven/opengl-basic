CFLAGS = -Wall -g
LIBS = -lglfw -ldl -lm

INCLUDES = -I external
BIN = build
OBJ = mesh.c shader.c graphics.c common.c image.c utils/file_handler.c
PROG = $(BIN)/p.exe

FEAT = mesh


.PHONY: clean dev


$(PROG): main.c $(OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(OBJ) main.c $(LIBS)


$(FEAT): $(FEAT).c $(OBJ)
	gcc -o $(BIN)/$(FEAT) $(CFLAGS) $(INCLUDES) $(OBJ) $(FEAT).c $(LIBS)


dev:
	apt install libglfw3 libglfw3-dev

clean:
	rm $(BIN)/*
