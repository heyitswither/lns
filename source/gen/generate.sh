#!/usr/bin/env bash
cd source
javac gen/generate.java
java gen.generate ./
if [ -f "gen/generate.class" ]; then
  rm gen/generate.class
fi
echo Done.