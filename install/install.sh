#!/usr/bin/env bash
cd ..
echo Compiling...
sudo g++ -w --permissive main.cpp -o ./lns
echo Installing...
sudo mv lns /usr/bin/lns
echo Installing man entry...
sudo cp manual/lns.1 /usr/share/man/man1/lns.1
sudo gzip /usr/share/man/man1/lns.1 -f
echo Installation completed. You can access the manual entry with the command "man lns"