# TeraPetTest1

## Build Instructions for a ubuntu desktop

Install the following packages to build the c++ source code:
> sudo apt install g++ build-essentials

Install the latest Boost library from https://www.boost.org/

Extract the following source code or clone it from GIT repository:
https://github.com/kefran74/TeraPetTest1

Make sure you adjust the makefile reference to your Boost installation folder if not in /usr/local/lib

Build the app:
> make clean all

Install the app:
> sudo make install

## usage

call the program "SortTimeStamp /my/data/file"

It will process it and produce a file called output.tsv with the sorted timestamps.

Enjoy