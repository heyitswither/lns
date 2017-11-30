#!/usr/bin/env bash
cd ..
javac generate.java
java generate ./
if [ -f "generate.class" ]; then
  rm generate.class
fi
echo Done.