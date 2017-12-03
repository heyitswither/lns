#!/usr/bin/env bash

#This file is used to compile the shared library files.

if [ $# -eq 1 ]; then
  g++ -Wl,--no-as-needed -ldl -fPIC -shared $1 -o lib.so
fi
