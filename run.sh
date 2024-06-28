#!/usr/bin/env bash

gcc main.c -o main -Wall -lSDL2 -lSDL2_mixer -lX11 -lXtst
./main
