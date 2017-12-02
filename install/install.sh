#!/bin/bash
cd ..

echo Removing previous versions...

sudo rm -f /usr/bin/lns
sudo rm -r -f /lns

sudo mkdir --parents /lns/lib

echo Compiling...
sudo g++ -w --permissive source/lns.cpp source/errors.h source/exceptions.h source/scanner.h source/defs.h source/parser.h source/options.h source/interpreter.h source/environments.h source/debug.h source/commands.h source/commands.cpp source/debug.cpp source/defs.cpp source/errors.cpp source/exceptions.cpp source/interpreter.cpp source/parser.cpp source/scanner.cpp source/stmt.h source/expr.h -o ./lns

echo Moving executable...
sudo mv lns /lns/

echo Creating link to executable...
sudo ln -s /lns/lns /usr/bin/

echo Moving libraries...
sudo cp -r lib /lns/

echo Done.
