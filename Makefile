CFLAGS = -Wall -g
LIBS = 

INCLUDES = -I external
BIN = build
PROG = $(BIN)/p.exe

UNAME_S := $(shell uname -s)


ifeq ($(UNAME_S), Linux)
	LIBS += `pkg-config --static --libs glfw3`
	CFLAGS += `pkg-config --cflags glfw3`
endif

ifeq ($(OS), Windows_NT)
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32`
	CFLAGS += `pkg-config --cflags glfw3`
endif


.PHONY: clean dev


$(PROG): main.c
	gcc -o $@ $(CFLAGS) $(INCLUDES) main.c $(LIBS)
	$(PROG)

dev: 
	apt install libglfw3 libglfw3-dev
  
clean: 
	rm $(BIN)/*