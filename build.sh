#!/bin/bash
clear
gcc src/main.c src/shader.c src/matvec.c src/entity.c -lglfw3 -lGL -lm -ldl -lXinerama -lXrandr -lXi -lXcursor -lX11 -lXxf86vm -lpthread   src/glad.c -o bin/game
