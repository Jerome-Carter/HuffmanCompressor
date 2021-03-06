#pragma once

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <bits/stdc++.h>

#include "node.h"

class HuffmanCompression
{
private:
    Node *tree;
    char pad_bits;
    std::string input_path;
    std::string output_path;
    std::string write_data;
    std::string encoded_data;
    int shortest_code_length;
    std::vector<char> *read_data;
    std::map<char, std::string> codes;
    std::map<std::string, char> reverse;
    std::map<char, unsigned long> frequency_map;
    std::priority_queue<Node *, std::vector<Node *>, MinHeapNodeComparator> min_heap;

public:
    HuffmanCompression();
    ~HuffmanCompression();
    void Compress(const std::string &input_path);
    void Decompress(const std::string &input_path, const std::string &output_path);

private:
    void ReadData();
    void WriteData() const;
    void ExtractEncodedData();
    void DecodeData();
    void MakeFrequencyMap();
    void FillMinHeap();
    void GenerateHuffmanTree();
    void GenerateHuffmanCodes(Node *root, const std::string &current_code);
    void PrintCodes() const;
    void EncodeData();
    void PadData();
};
