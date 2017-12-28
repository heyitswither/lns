#!/bin/bash
CHK(){
    if [ $1 -ne 0 ]; then
        printf "\n"
        echo "$2. Exiting." & exit
    fi
}
cd "${0%/*}" #make sure we're in the right directory
cd ..

sudo printf "Removing symbolic link (if present)..."
sudo rm -f /usr/bin/lns
printf " done\n"

printf "Removing previous installation folder (if present)..."
sudo rm -r -f /lns
printf " done\n"

printf "Creating install directory..."
sudo mkdir --parents /lns/lib
CHK $? "Unable to create install directory"
printf " done\n"

printf "Compiling executable..."
sudo g++ -w -Wall -rdynamic -Wl,--no-as-needed -ldl source/*.cpp -o ./lns
CHK $? "\nCompilation failed"
printf " done\n"

printf "Moving executable..."
sudo mv lns /lns/
CHK $? "Unable to move the executable"
printf " done\n"

printf "Creating link to executable in /usr/bin..."
sudo ln -s /lns/lns /usr/bin/
cp source/defs.h lib/natives/src/
CHK $? "Unable to create link to executable"
printf " done\n"

cd lib/natives/
printf "Removed any previously compiled .so files..."
for f in *.so
do
    if [ $f = "*.so" ]; then
        break
    fi
    rm -f $f
done
printf " done\n"

cd src

echo "Compiling native libraries..."
for f in *.cpp
do
    printf "Compiling $f..."
    fname=${f%.*}
    sudo g++ -w -Wl,--no-as-needed -ldl -fPIC -shared $f -o lib.so
    CHK $? "Compilation of file $f failed"
    mv lib.so ../$fname.so
    printf " done.\n"
done

cd ../../..

sudo rm lib/natives/src/defs.h

printf "Moving libraries..."
sudo cp -r lib /lns/
CHK $? "Unable to move libraries"
printf " done\n"

printf "Removing sources from installation directory..."
sudo rm -r /lns/lib/natives/src
printf " done\n"

printf "Installing manual entry..."
sudo cp manual/lns.1 /usr/share/man/man1/
printf " done\n"

echo "Installation completed."
