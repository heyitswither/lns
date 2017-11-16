#!/bin/bash
cd ..
cd source
echo Compiling...
sudo g++ -w --permissive lns.cpp -o ./lns
echo Installing...
sudo mv lns /usr/bin/lns
echo Installing man entry...
cd ..
sudo cp manual/lns.1 /usr/share/man/man1/lns.1
sudo gzip /usr/share/man/man1/lns.1 -f
echo Installation completed. You can access the manual entry with the command "man lns"
