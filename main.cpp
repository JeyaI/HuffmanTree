#include "huffmantree.h"
#include <string>
#include <iostream>
#include <algorithm>

int main(int argc, char** argv){
    if(argc < 3){
        std::cout << "Not enough arguments!" << std::endl;
        return -1;
    }

    std::string mode = argv[1];

    std::transform(mode.begin(), mode.end(), mode.begin(),
    [](unsigned char c){ return std::tolower(c); });

    if(mode == "-c"){
        
        if(argc >= 4){
            HuffmanTree::compressFile(argv[2], argv[3]);
        }else{
            HuffmanTree::compressFile(argv[2]);
        }

    }else if(mode == "-d"){

        if(argc >= 4){
            HuffmanTree::decompressFile(argv[2], argv[3]);
        }else{
            HuffmanTree::decompressFile(argv[2]);
        }

    }else{
        std::cout << "Improper mode provided!" << std::endl;
        return -2;
    }
}