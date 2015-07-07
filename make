#!/bin/bash

reset
rm ./a.out
g++ -std=c++11 *.cpp *.h
./a.out