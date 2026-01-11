Yo infinite snek nice

Linux setup:
Go into raylib/src, then run `make`<br >
After that compile and run project with:<br>
`gcc -Iraylib/src/ -o main main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Lraylib/src/ -l:libraylib.a && ./main`
