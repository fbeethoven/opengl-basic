CFLAGS = -Wall -g
LIBS = -lglfw -ldl -lm

INCLUDES = -I external
BIN = build
EXPERIMENT_DIR = experiments
OBJ = memory.c font.c mesh.c shader.c graphics.c common.c image.c \
	  utils/helpers.c utils/file_handler.c
PROG = $(BIN)/prog

RANDOM_OBJ = memory.c font.c mesh.c shader.c graphics.c common.c image.c \
	utils/helpers.c utils/file_handler.c experiments/utils/experiment_helpers.c
RANDOM = $(BIN)/rand


.PHONY: clean dev


all: $(RANDOM)

$(PROG): main.c $(OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(OBJ) main.c $(LIBS)

$(RANDOM): $(EXPERIMENT_DIR)/random.c $(RANDOM_OBJ)
	gcc -o $@ $(CFLAGS) $(INCLUDES) $(RANDOM_OBJ) \
		$(EXPERIMENT_DIR)/random.c $(LIBS)

dev:
	apt install libglfw3 libglfw3-dev

clean:
	rm $(BIN)/*
