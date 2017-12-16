#!/bin/bash

cd "${0%/*}" #make sure we're in the right directory
cd ..

echo "Removing symbolic link (if present)..."
sudo rm -f /usr/bin/lns

echo "Removing previous installation folder (if present)..."
sudo rm -r -f /lns

echo "Creating install directory..."
sudo mkdir --parents /lns/lib

echo "Compiling executable..."
sudo g++ -w -Wall -fpermissive -rdynamic -Wl,--no-as-needed -ldl source/lns.cpp source/errors.h source/exceptions.h source/scanner.h source/defs.h source/parser.h source/options.h source/interpreter.h source/debug.h source/commands.h source/commands.cpp source/debug.cpp source/defs.cpp source/errors.cpp source/exceptions.cpp source/interpreter.cpp source/parser.cpp source/scanner.cpp source/options.cpp -o ./lns

echo "Moving executable..."
sudo mv lns /lns/

echo "Creating link to executable in /usr/bin..."
sudo ln -s /lns/lns /usr/bin/

cp source/defs.h lib/natives/src/

cd lib/natives/
echo "Removed any previously compiled .so files..."
for f in *.so
do
    if [ $f = "*.so" ]; then
        break
    fi
    rm -f $f
done

cd src

echo "Compiling native libraries..."
for f in *.cpp
do
    printf "Compiling $f..."
    fname=${f%.*}
    sudo g++ -w -Wl,--no-as-needed -ldl -fPIC -shared $f -o lib.so
    mv lib.so ../$fname.so
    printf " done.\n"
done

cd ../../..

sudo rm lib/natives/src/defs.h

echo "Moving libraries..."
sudo cp -r lib /lns/

echo "Removing sources from installation directory..."
sudo rm -r /lns/lib/natives/src

echo "Done."
