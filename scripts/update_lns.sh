#!/bin/bash

CHK(){
    if [ $1 -ne 0 ]; then
        printf "\n"
        echo "$2. Exiting." & exit 1
    fi
}


if [ $(which git | wc -l) -eq 0 ]; then
	echo "Git is not installed. Install it by typing 'sudo apt-get install git'"
	exit 1
fi

cd ~
rm -f -r lns_temp

printf "Cloning the repository..."
git clone https://github.com/lorenzonotaro/lns.git lns_temp
CHK $? "Unable to clone the repository"
printf "done.\n"

echo "Calling the install script..."
bash "lns_temp/scripts/install_local.sh"

rm -f -r lns_temp