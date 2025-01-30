#ifndef HUFFMANTREE
#define HUFFMANTREE

#include <vector>
#include <iostream>

static const char defaultExtension[] = ".hc";

class HuffmanTree {
public:
    HuffmanTree(int* frequencies, bool decorate = false);
    HuffmanTree(const char* src, int size, bool decorate = true);

    ~HuffmanTree();

    void print();

    static void compressFile(const char* filename, const char* extension = defaultExtension);
    static void decompressFile(const char* inFilename, const char* outFilename = nullptr);

private:
    struct BoolChain {
        std::vector<bool> bools;

        void print() {
            for (auto curr : bools) {
                std::cout << (curr ? 1 : 0);
            }
        }

        void addTo(std::vector<bool>& encoded) {
            encoded.insert(encoded.end(), bools.begin(), bools.end());
        }
    };
    struct TreeNode;
    struct TreeNodeCompare;

    BoolChain encodeMap[256];
    int freq[256] = {0};
    int _size;
    TreeNode* root;

    TreeNode* combineTreeNodes(TreeNode* left, TreeNode* right);
    void attachBoolChains();
    void attachBoolChainsHelper(TreeNode* apply, std::vector<bool>& bools);
    void print(TreeNode* node, int tabs);
    void createSelfFromFrequencies();
    
};

#endif