#!/usr/bin/env bash
cd source
rm generate.class
javac generate.java
java generate ./
echo Done.