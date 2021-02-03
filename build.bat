@echo off

set defines=-D__DEBUG___ -O2 -D_CRT_SECURE_NO_WARNINGS
set includes=-I"C:\Program Files\glew\src" -L"C:\Program Files\glew\lib"
set libraries=-lopengl32 -lglew32s -luser32 -lgdi32 

clang++ -o bin\win32.exe src\Windows.cpp %defines% %includes% %libraries%
