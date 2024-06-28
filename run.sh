#!/usr/bin/env bash

g++ main.c -o main -lSDL2 -lSDL2_mixer -lX11 -lXtst
./main
