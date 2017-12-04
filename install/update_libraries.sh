#!/usr/bin/env bash
cd "${0%/*}" #make sure we're in the right directory
cd ..
echo Moving libraries...
sudo cp -r lib /lns/