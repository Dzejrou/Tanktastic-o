#!/bin/sh

if [ ! -d SFML ]; then
    mkdir SFML
fi

if [ ! -d SFML/include ]; then
    mkdir SFML/include
fi

cd SFML && cmake -G "Unix Makefiles" ../deps/SFML-2.2 && cp -r ../deps/SFML-2.2/include/* include/ && make
cd .. && make all
