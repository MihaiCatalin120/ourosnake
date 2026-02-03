Yo infinite snek nice

Linux setup:
Go into raylib/src, then run `make`<br>

After that compile and run project with:<br>
`gcc -Iraylib/src/ -o main all.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Lraylib/src/ -l:libraylib.a && ./main`

Mac setup:
Go into raylib/src, then run `make`<br>

Copy main.c into raylib/src<br>

Then go into raylib/src and run project with: <br>
`gcc -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL libraylib.a all.c -o main && ./main`

Windows build:
Install mingw-gcc compiler

Download raylib release from github: https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_win64_mingw-w64.zip

Extract and rename folder as raylib_win

Build the executable with:
`x86_64-w64-mingw32-gcc all.c -o ourosnake.exe -I ./raylib_win/include -L ./raylib_win/lib -lraylib -lgdi32 -lwinmm -mwindows`

Then run the game by opening ourosnake.exe


In order to debug don't forget to add the -g flag to the compiler so breakpoints are not ignored :)

Enjoy :D
