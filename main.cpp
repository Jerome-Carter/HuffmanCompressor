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

struct Node
{
    char* ch = nullptr;
    unsigned long frequency = 0;
    Node *left = nullptr;
    Node *right = nullptr;
    friend std::ostream &operator<<(std::ostream &os, const Node &n)
    {
        os << (*(n.ch) != '\n' && *(n.ch) != '\0' ? std::string(1, *(n.ch)) : "") << (*(n.ch) != '\0' ? "," : "") << n.frequency;
        return os;
    }
};

void DeleteNode(Node* node)
{
    if (node == nullptr) return;
    DeleteNode(node->left); 
    DeleteNode(node->right);
    delete node->ch;
    delete node;  
}

// Thanks to GeeksForGeeks
void PrintTree(Node *root, unsigned int indent = 0, unsigned int indent_size = 8)
{
    if (root == nullptr)
        return;

    indent += indent_size;
    PrintTree(root->right, indent);
    std::cout << std::endl;
    for (int i = indent_size; i < indent; i++)
        std::cout << " ";
    std::cout << *root << std::endl;
    PrintTree(root->left, indent);
}

class MinHeapComparator
{
public:
    int operator()(Node *p1, Node *p2)
    {
        return p1->frequency > p2->frequency;
    }
};

class HuffmanCompression
{
private:
    Node* tree;
    std::string input_path;
    std::string output_path;
    std::string write_data;
    std::string encoded_data;
    std::vector<char> read_data;
    std::map<char, std::string> codes;
    std::map<std::string, char> reverse;
    std::map<char, unsigned long> frequency_map;
    std::priority_queue<Node *, std::vector<Node *>, MinHeapComparator> min_heap;

public:
    HuffmanCompression()
     : tree(nullptr)
    {
    }
    ~HuffmanCompression()
    {
        DeleteNode(tree);
    }
    void Compress(const std::string &input_path)
    {
        this->input_path = input_path;
        std::size_t end_path = input_path.find_last_of("/\\");
        std::string path = input_path.substr(0, end_path);
        std::string file = input_path.substr(end_path);
        std::size_t begin_ext = file.find(".");
        std::string file_name = file.substr(0, begin_ext);
        std::string file_ext = file.substr(begin_ext);
        this->output_path = path + file_name + ".bin";

        ReadData();

        MakeFrequencyMap();
        FillMinHeap();
        GenerateHuffmanTree();
        std::string current_code = "";
        GenerateHuffmanCodes(this->tree, current_code);

        EncodeData();
        PadData();
        WriteData();

        PrintCodes();
    }
    void Decompress(const std::string &input_path, const std::string &output_path)
    {
        this->input_path = input_path;
        this->output_path = output_path;
        ReadData();
        ExtractEncodedData();
        DecodeData();
        WriteData();
    }

private:
    void ReadData()
    {
        this->read_data.clear();
        std::ifstream ifile(this->input_path, std::ios::in | std::ios::binary);
        ifile.unsetf(std::ios::skipws);

        std::streampos fileSize;
        ifile.seekg(0, std::ios::end);
        fileSize = ifile.tellg();
        ifile.seekg(0, std::ios::beg);

        // reserve capacity
        this->read_data.reserve(fileSize);

        // read the data:
        this->read_data.insert(this->read_data.begin(),
                               std::istream_iterator<char>(ifile),
                               std::istream_iterator<char>());
    }
    void ExtractEncodedData()
    {
        char pad_length = this->read_data[0];
        std::string compressed_data = "";
        for (unsigned int i = 0; i < this->read_data.size(); i++)
            compressed_data += std::bitset<8>(this->read_data[i]).to_string();

        compressed_data.erase(0, 8);
        if (pad_length > 0)
            compressed_data.erase(compressed_data.size() - pad_length);
        this->encoded_data = compressed_data;
    }
    void DecodeData()
    {
        this->write_data = "";
        std::string current_code = "";
        for (char b : this->encoded_data)
        {
            current_code += b;
            if (this->reverse.find(current_code) != this->reverse.end())
            {
                char c = this->reverse[current_code];
                this->write_data += std::bitset<8>(c).to_string();
                current_code = "";
            }
        }
    }
    void MakeFrequencyMap()
    {
        this->frequency_map.clear();
        // Changing this made great time improvements
        // There may be more optimization possible here
        for (const char &c : this->read_data)
            this->frequency_map[c]++;
    }
    void FillMinHeap()
    {
        for (const std::pair<char, unsigned long> &p : this->frequency_map)
        {
            Node *n = new Node;
            n->ch = new char;
            *(n->ch) = p.first;
            n->frequency = p.second;
            this->min_heap.push(n);
        }
    }
    void GenerateHuffmanTree()
    {
        while (this->min_heap.size() > 1)
        {
            Node *n1 = this->min_heap.top();
            this->min_heap.pop();
            Node *n2 = this->min_heap.top();
            this->min_heap.pop();

            Node *merged = new Node;
            merged->frequency = n1->frequency + n2->frequency;
            merged->left = n1;
            merged->right = n2;
            this->min_heap.push(merged);
        }
        this->tree = this->min_heap.top();
        this->min_heap.pop();
    }
    void GenerateHuffmanCodes(Node *root, const std::string &current_code)
    {
        if (root == nullptr)
            return;

        // Default of \0 in Node is causing data loss, fix this
        if (root->ch != nullptr)
        {
            this->codes[*(root->ch)] = current_code;
            this->reverse[current_code] = *(root->ch);
            return;
        }

        GenerateHuffmanCodes(root->left, current_code + "0");
        GenerateHuffmanCodes(root->right, current_code + "1");
    }
    void PrintCodes()
    {
        for (std::map<std::string, char>::iterator i = this->reverse.begin(); i != this->reverse.end(); i++)
        {
            std::cout << i->first << " " << std::bitset<8>(i->second).to_string() << "\n";
        }
    }
    void EncodeData()
    {
        for (char i : this->read_data)
            this->encoded_data.append(codes[i]);
    }
    void PadData()
    {
        this->write_data = this->encoded_data;
        unsigned int pad_length = 8 - (this->write_data.size() % 8);
        this->write_data.append(pad_length, '0');
        std::bitset<8> pad_binary(pad_length);
        this->write_data.insert(0, pad_binary.to_string());
    }
    void WriteData()
    {
        std::ofstream outfile;
        outfile.open(this->output_path, std::ios::out | std::ios::binary);
        for (unsigned int i = 0; i < this->write_data.size(); i += 8)
        {
            int b = stoi(this->write_data.substr(i, 8), 0, 2);
            outfile.write(reinterpret_cast<const char *>(&b), 1);
        }
        outfile.close();
    }
};

int main(int argc, char *argv[])
{
    HuffmanCompression hc;
    // hc.Compress("/Users/james/Mahlet/Basic_Concole_App copy/README.md");
    auto start = std::chrono::high_resolution_clock::now();
    hc.Compress("/Users/james/Mahlet/Basic_Concole_App copy/app.out");
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Compression took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
              << " mseconds\n";
    start = std::chrono::high_resolution_clock::now();
    hc.Decompress("/Users/james/Mahlet/Basic_Concole_App copy/app.bin", "/Users/james/Mahlet/Basic_Concole_App copy/app.1.out");
    finish = std::chrono::high_resolution_clock::now();
    std::cout << "Decompression took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
              << " mseconds\n";
    return 0;
}
