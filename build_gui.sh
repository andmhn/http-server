#!/bin/bash

BUILD="bin/gui"
mkdir -p $BUILD

echo "build the server backend sources..."
gcc -g -c src/parser.c   -o $BUILD/parser.o -Isrc
gcc -g -c src/server.c   -o $BUILD/server.o -Isrc
gcc -g -c src/utils.c    -o $BUILD/utils.o  -Isrc
gcc -g -c src/folders.c  -o $BUILD/folder.o -Isrc

echo "now build the gui sources..."
g++ -g -c gui/main.cpp   -o $BUILD/main.o   -Igui
g++ -g -c gui/window.cpp -o $BUILD/window.o -Igui

echo "Linking..."
g++ -o bin/http-server-gui $BUILD/*.o `fltk-config --use-gl --use-images --ldstaticflags`

echo "build compelete"
echo "Executable: bin/http-server-gui"
