#!/bin/bash

# clear
g++ -o sudoku sudoku_solver.cpp
./sudoku $1 $2
