#!/usr/bin/env bash

g++ main.cpp -o main.out -lSDL2 -lSDL2_mixer -lX11 -lXtst
./main.out
