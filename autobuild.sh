#!/bin/bash

set -e

if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

rm -rf `pwd`/build/*

cd `pwd`/build && cmake .. && make && make install

cd ..

# ldconfig

echo "build success! you can use 'ldconfig' to update your ld config."
