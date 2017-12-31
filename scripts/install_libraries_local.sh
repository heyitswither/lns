#!/bin/bash

CHK(){
    if [ $1 -ne 0 ]; then
        printf "\n"
        echo "$2. Exiting." & exit 1
    fi
}

cd "${0%/*}" #make sure we're in the right directory
cd ..

sudo rm -f -r /lns/lib
sudo mkdir /lns/lib
sudo cp source/defs.h lib/natives/src/

cd lib/
echo "Performing syntax check on libraries..."
for f in *.lns
do
    printf "Checking file $f..."
    lns --parse-only --ignore-unresolved ${f}
    CHK $? "File $f has syntax errors"
    printf " done\n"
done

cd natives/
printf "Removed any previously compiled .so files..."
for f in *.so
do
    if [ ${f} = "*.so" ]; then
        break
    fi
    rm -f ${f}
done
printf " done\n"

cd src

echo "Compiling native libraries..."
for f in *.cpp
do
    printf "Compiling file $f..."
    fname=${f%.*}
    sudo g++ -w -Wl,--no-as-needed -ldl -fPIC -shared ${f} -o lib.so
    CHK $? "Compilation of file $f failed"
    mv lib.so ../${fname}.so
    printf " done.\n"
done

cd ../../..

sudo rm lib/natives/src/defs.h

printf "Copying libraries..."
sudo cp -r lib /lns/
CHK $? "Unable to move libraries"
printf " done\n"

printf "Removing sources from installation directory..."
sudo rm -r /lns/lib/natives/src
printf " done\n"