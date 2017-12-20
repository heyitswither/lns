#!/usr/bin/env bash
cd source
javac gen/generate.java

if [ $? -ne 0 ]; then
    echo "Compilation failed." & exit
fi

java gen.generate ./

rm -f gen/generate.class

echo Done.