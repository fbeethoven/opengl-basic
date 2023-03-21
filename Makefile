CFLAGS = -Wall -g
LIBS = -lglfw -ldl

INCLUDES = -I external
BIN = build
PROG = $(BIN)/win


.PHONY: clean dev run


$(PROG): main.c
	gcc -o $@ $(CFLAGS) $(INCLUDES) main.c $(LIBS)

dev: 
	apt install libglfw3 libglfw3-dev
  
clean: 
	rm $(BIN)/*
