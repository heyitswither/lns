#!/bin/bash

RESULT=$(curl -s https://raw.githubusercontent.com/lorenzonotaro/lns/master/source/revision.h | grep -o "[0-9]*$")
if [ -z $RESULT ]; then
    exit 1
fi

if [ $1 -lt $RESULT ]; then
   echo "A newer version of lns is available (rev. $RESULT). Launch lns with the --update option to download it."
fi