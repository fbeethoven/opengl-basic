set CC="gcc"
set CFLAGS="-Wall -Wextra -g"
set LIBS="-lglfw3 -lgdi32 -lopengl32 -limm32"

set INCLUDES="-I external"
set BIN="build"
set OBJ="shader.c graphics.c common.c image.c utils//file_handler.c"
set PROG="%BIN%//p.exe"


gcc -o "%PROG%" "%CFLAGS%" "%INCLUDES%" "%OBJ%" main.c "%LIBS%"

