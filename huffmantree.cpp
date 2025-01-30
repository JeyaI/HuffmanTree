#include "huffmantree.h"
#include <iostream>
#include <queue>
#include <fstream>
#include <sstream>
#include <string>

struct HuffmanTree::TreeNode{
    bool isLeaf;
    char key;
    int count;
    TreeNode* left;
    TreeNode* right;
    BoolChain boolChain;

    TreeNode(bool _isLeaf, char _key, int _count, TreeNode* _left, TreeNode* _right) :
        isLeaf(_isLeaf),
        key(_key),
        count(_count),
        left(_left),
        right(_right)
        {
        }

    ~TreeNode(){
        delete left;
        delete right;
        //delete boolChain;
    }
};

HuffmanTree::HuffmanTree(int* frequencies, bool decorate){
    for(int i = 0; i < 256; i++){
        freq[i] = frequencies[i];
    }

    createSelfFromFrequencies();

    if(decorate){
        attachBoolChains();
    }
}

HuffmanTree::HuffmanTree(const char* src, int size, bool decorate){
    _size = size; //may be useless, realized while refactoring

    unsigned char* curr = (unsigned char*)src;

    while(size > 0){
        size--;
        freq[*curr]++;
        curr++;
    }

    createSelfFromFrequencies();

    if(decorate){
        attachBoolChains();
    }
}

HuffmanTree::~HuffmanTree(){
    delete root;
}

void HuffmanTree::print(){
    print(root, 0);
    std::cout << std::flush;
}
void HuffmanTree::print(TreeNode* node, int tabs){
    if(!node){
        return;
    }
    
    for(int i = 0 ; i < tabs; i++){
        std::cout << "\t";
    }

    std::cout <<  "( "  << node->key << " : " << node->count << " )";
    
    if(!node->boolChain.bools.empty()){
        node->boolChain.print();
    }

    std::cout << "\n";

    print(node->left, tabs+1);
    print(node->right, tabs+1);
}

HuffmanTree::TreeNode* HuffmanTree::combineTreeNodes(TreeNode* left, TreeNode* right){
    if(!left || !right){
        return nullptr;
    }

    return new TreeNode{false, 0, left->count + right->count, left, right};
}

void HuffmanTree::attachBoolChains(){
    TreeNode* curr = root;

    std::vector<bool> bools;

    attachBoolChainsHelper(curr, bools);
}

void HuffmanTree::attachBoolChainsHelper(TreeNode* apply, std::vector<bool>& boolChain){
    if(!apply){
        return;
    }

    //apply->boolChain->bools = boolChain;
    std::vector<bool> lCopy = boolChain;

    if(apply->isLeaf){
        encodeMap[(unsigned char)apply->key].bools = boolChain;
        apply->boolChain.bools = boolChain;
    }

    lCopy.push_back(false);
    boolChain.push_back(true);

    attachBoolChainsHelper(apply->left, lCopy);
    attachBoolChainsHelper(apply->right, boolChain);
}

struct HuffmanTree::TreeNodeCompare{
    bool operator()(const HuffmanTree::TreeNode* lhs, const HuffmanTree::TreeNode* rhs){
        return lhs->count > rhs->count;
    }
};

void HuffmanTree::createSelfFromFrequencies(){
    std::priority_queue<TreeNode*, std::vector<TreeNode*>, TreeNodeCompare> pq;

    for(int i = 0; i < 256; i++){
        if(freq[i] != 0){
            pq.push(new TreeNode(true, i, freq[i], nullptr, nullptr));
        }
    }

    //pop 2 off and combine
    //eventually need to handle only 1 symbol case
    while(pq.size() >= 2){
        TreeNode* theLeft = pq.top();
        pq.pop();
        TreeNode* theRight = pq.top();
        pq.pop();

        pq.push(combineTreeNodes(theLeft, theRight));
    }

    //if here, the size is 1.
    root = pq.top();
    pq.pop();
}

void HuffmanTree::compressFile(const char* filename, const char* extension){
    std::ifstream inputfile(filename, std::ios::binary);

    std::stringstream inputBuffer;

    inputBuffer << inputfile.rdbuf();

    inputfile.close();

    std::vector<bool> encodedStream;

    std::string inputString = inputBuffer.str();

    int uncompressedSize = inputString.size();

    HuffmanTree huffmanTree(inputString.c_str(), inputString.size());

    for(auto c : inputString){
        huffmanTree.encodeMap[(unsigned char)c].addTo(encodedStream);
    }

    // I know std::vector<bool> is compacted, but I'm keeping this in for expressivity
    std::vector<unsigned char> encodedStreamCompressed;

    for(int i = 0; i < encodedStream.size(); i += 8){
        int curr = i;
        unsigned char theByte = 0;

        while(curr < encodedStream.size() && curr - i < 8){
            theByte <<= 1;
            theByte += (encodedStream[curr]? 1 : 0);
            curr++;
        }

        //on the last one, we are going to want to make sure it is "pushed up" to the top bits of the byte
        while(curr - i < 8){
            theByte <<= 1;
            curr++;
        }

        encodedStreamCompressed.push_back(theByte);
    }

    std::string fileOutName = filename;

    fileOutName += extension;

    auto outputFile = std::fstream(fileOutName.c_str(), std::ios::out | std::ios::binary);

    outputFile.write((const char*)huffmanTree.freq, sizeof(huffmanTree.freq));

    outputFile.write((const char*)&uncompressedSize, sizeof(uncompressedSize));

    outputFile.write((const char*)encodedStreamCompressed.data(), sizeof(unsigned char) * encodedStreamCompressed.size());

    outputFile.close();
}

void HuffmanTree::decompressFile(const char* inFilename, const char* outFilename){
    std::ifstream inputFile(inFilename, std::ios::binary);

    std::stringstream buffer;

    buffer << inputFile.rdbuf();

    inputFile.close();

    auto str = buffer.str();

    char* inputCStr = (char*)str.c_str();

    int* freqs = reinterpret_cast<int*>(inputCStr);

    HuffmanTree huffmanTree(freqs);

    str = str.substr(sizeof(huffmanTree.freq));

    const char* sizeCStr = str.c_str();

    int size = *reinterpret_cast<const int*>(sizeCStr);

    str = str.substr(sizeof(int));

    std::vector<unsigned char> decoded;

    std::queue<bool> processing;

    for(auto c : str){
        processing.push(c & 0b10000000);
        processing.push(c & 0b01000000);
        processing.push(c & 0b00100000);
        processing.push(c & 0b00010000);
        processing.push(c & 0b00001000);
        processing.push(c & 0b00000100);
        processing.push(c & 0b00000010);
        processing.push(c & 0b00000001);
    }

    HuffmanTree::TreeNode* currNode = nullptr;

    while(size != 0 && !processing.empty()){
        if(!currNode){
            currNode = huffmanTree.root;
        }

        bool direction = processing.front();

        processing.pop();

        if(direction == true){ //right
            currNode = currNode->right;
        }else{
            currNode = currNode->left;
        }

        if(currNode->isLeaf){
            decoded.push_back(currNode->key);
            currNode = nullptr;
            size--;
        }
    }

    std::string defaultOutFilename;

    if(!outFilename){
        defaultOutFilename = inFilename;

        defaultOutFilename =  defaultOutFilename.substr(0, defaultOutFilename.find_last_of('.'));

        outFilename = defaultOutFilename.c_str();
    }

    auto outputFile = std::fstream(outFilename, std::ios::out | std::ios::binary);

    outputFile.write((const char*)decoded.data(), decoded.size());

    outputFile.close();

}