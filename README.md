# HuffmanTree
## About
This repository contains a very simple implementation of a Huffman tree, and it is used here to make a simple compression utility. I'm pretty sure there a few bugs in this implementation (patches welcome!), but I wrote this as an evening after-work learning exercise, so I'm not too concerned about that.

## Building
There is a very simple CMakeLists.txt provided here, but you will probably be able to figure out how to build this any way you want on your own.

A g++ example: 

    g++ -o hc HuffmanTree.cpp main.cpp

## Using the command

- hc -c [file to be compressed] [appended file extension (optional)]
- hc -d [file to be decompressed] [file name for decompressed output (optional)]

Examples:

    hc -c main.cpp 
Compress the file "main.cpp" and save resulting file as "main.cpp.hc".

    hc -d main.cpp.hc
Decompress the file "main.cpp.hc" and save resulting file as "main.cpp".

    hc -c main.cpp .jz
Compress the file "main.cpp" and save resulting file as "main.cpp.jz".

    hc -d main.cpp.hc decompressed.txt
Decompress the file "main.cpp.jz" and save resulting file as "decompressed.txt". 
