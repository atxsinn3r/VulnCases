#!/bin/bash

sudo bash -c 'echo 0 > /proc/sys/kernel/randomize_va_space'
g++ webserver.cpp -o webserver -std=c++11 -lpistache -lpthread -fno-stack-protector