Yo infinite snek nice

Linux setup:
Install raylib, then build it in whatever way you what
After that compile and run project with:
`gcc -Iraylib-5.5/src/ -o main main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Lraylib-5.5/src/ -l:libraylib.a && ./main`
