CFLAGS = -Wall -g
LIBS = -lglfw -ldl -lm

INCLUDES = -I ../external
BIN = ../build
OBJ = memory.c font.c mesh.c shader.c graphics.c common.c image.c \
	  utils/helpers.c utils/file_handler.c
PROG = $(BIN)/p

EXP_DIR = experiments
PACKMAN_OBJ = memory.c font.c mesh.c shader.c graphics.c common.c image.c \
	utils/helpers.c utils/file_handler.c $(EXP_DIR)/utils/experiment_helpers.c
PACKMAN = $(BIN)/packman


.PHONY: clean dev all packman

all: clean $(PROG)
	$(PROG)

packman: clean $(PACKMAN)
	$(PACKMAN)

$(PROG): main.c $(OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(OBJ) main.c $(LIBS)

$(PACKMAN): $(EXP_DIR)/random.c $(PACKMAN_OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(PACKMAN_OBJ) \
		$(EXP_DIR)/random.c $(LIBS)

dev:
	apt install libglfw3 libglfw3-dev

clean:
	rm $(BIN)/*
