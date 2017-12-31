#!/bin/bash

#check if a newer version of lns is available
#returns:
#    1 if the interpreter is up to date
#    2 if an update is available
#    3 if an error occurred


RESULT=$(curl -s https://raw.githubusercontent.com/lorenzonotaro/lns/master/source/revision.h | grep -o "[0-9]*$")
if [ -z $RESULT ]; then
    exit 3
fi

if [ $1 -lt $RESULT ]; then
   printf "A newer version of lns is available (rev. $RESULT). "
   exit 2
fi

exit 1