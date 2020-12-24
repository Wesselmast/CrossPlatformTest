#!/usr/bash

includes="-L/usr/X11/lib -L/usr/include/GL" 
libraries="-lX11 -lGLEW -lGL -lGLU"

g++ -o bin/out Linux.cpp $includes $libraries
