CFLAGS = -Wall -g
LIBS = -lglfw -ldl

INCLUDES = -I external
BIN = build
PROG = $(BIN)/win


.PHONY: clean dev


$(PROG): main.c
	gcc -o $@ $(CFLAGS) $(INCLUDES) main.c $(LIBS)
	$(PROG)

dev: 
	apt install libglfw3 libglfw3-dev
  
clean: 
	rm $(BIN)/*
