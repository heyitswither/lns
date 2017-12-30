#!/bin/bash
CHK(){
    if [ $1 -ne 0 ]; then
        printf "\n"
        echo "$2. Exiting." & exit 1
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
CHK $? "Unable to create link to executable"
printf " done\n"

cd install
sudo bash ./install_libraries.sh

cd ..
printf "Installing manual entry..."
sudo cp manual/lns.1 /usr/share/man/man1/
printf " done\n"

echo "Installation completed."
