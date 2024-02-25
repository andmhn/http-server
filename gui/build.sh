# parser.c logger.c server.c utils.c folders.c
gcc -g -c parser.c
gcc -g -c server.c
gcc -g -c utils.c
gcc -g -c folders.c

g++ -g -o http-server  main.cpp window.cpp\
    parser.o server.o utils.o folders.o\
    `fltk-config --use-gl --use-images --ldstaticflags`
