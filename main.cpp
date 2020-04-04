#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <chrono>

#include "huffman_compression.h"

// Thanks to GeeksForGeeks
// void PrintTree(Node *root, unsigned int indent = 0, unsigned int indent_size = 8)
// {
//     if (root == nullptr)
//         return;

//     indent += indent_size;
//     PrintTree(root->right, indent);
//     std::cout << std::endl;
//     for (int i = indent_size; i < indent; i++)
//         std::cout << " ";
//     std::cout << *root << std::endl;
//     PrintTree(root->left, indent);
// }

int main(int argc, char *argv[])
{
    HuffmanCompression hc;
    // hc.Compress("/Users/james/Mahlet/Basic_Concole_App copy/README.md");
    auto start = std::chrono::high_resolution_clock::now();
    hc.Compress("/Users/james/Mahlet/Basic_Concole_App copy/main.cpp");
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Compression took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
              << " milliseconds\n";
    start = std::chrono::high_resolution_clock::now();
    hc.Decompress("/Users/james/Mahlet/Basic_Concole_App copy/main.bin", "/Users/james/Mahlet/Basic_Concole_App copy/main.out");
    finish = std::chrono::high_resolution_clock::now();
    std::cout << "Decompression took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
              << " milliseconds\n";
    return 0;
}
