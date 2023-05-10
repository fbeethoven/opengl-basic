CFLAGS = -Wall -g
LIBS = -lglfw -ldl -lm

INCLUDES = -I ../external
BIN = ../build
EXP_DIR = experiments
OBJ = memory.c font.c mesh.c shader.c graphics.c common.c image.c \
	  utils/helpers.c utils/file_handler.c \
	  $(EXP_DIR)/packman.c $(EXP_DIR)/utils/experiment_helpers.c
PROG = $(BIN)/p
GLTF = $(BIN)/gltf


.PHONY: clean dev all main tmp

all: clean $(PROG)
	$(PROG)

main: $(PROG)
	$(PROG)

tmp: $(GLTF)
	$(GLTF)

$(PROG): main.c $(OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(OBJ) main.c $(LIBS)

$(GLTF): utils/file_handler.c memory.c
	gcc -o $@ $(CFLAGS) $(INCLUDES) memory.c utils/file_handler.c $(LIBS)

dev:
	apt install libglfw3 libglfw3-dev

clean:
	rm $(BIN)/*
