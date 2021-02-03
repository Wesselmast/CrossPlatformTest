#!/usr/bash

defines="-D__DEBUG__ -g"
includes="-L/usr/X11/lib -L/usr/include/GL" 
libraries="-lX11 -lGLEW -lGL -lGLU"

g++ -o bin/linux src/Linux.cpp $defines $includes $libraries
