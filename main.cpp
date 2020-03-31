#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <bits/stdc++.h>

struct Node
{
    char ch = '\0';
    unsigned int frequency = 0;
    Node *left = nullptr;
    Node *right = nullptr;
    friend std::ostream& operator<<(std::ostream& os, const Node& n) {
        os << (n.ch != '\n' && n.ch != '\0' ? std::string(1, n.ch) : "") << (n.ch != '\0' ? "," : "") << n.frequency;
        return os;
    }
};

// Thanks to GeeksForGeeks
void PrintTree(Node* root, unsigned int indent = 0, unsigned int indent_size = 8) {
    if (root == nullptr)
        return;
    
    indent += indent_size;
    PrintTree(root->right, indent);
    std::cout << std::endl;
    for (int i = indent_size; i < indent; i++)  
        std::cout<< " ";
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
    std::string data;
    std::string input_path;
    std::string output_path;
    std::string padded_data;
    std::string encoded_data;
    std::map<char, std::string> codes;
    std::map<std::string, char> reverse;
    std::map<char, unsigned long> frequency_map;
    std::priority_queue<Node *, std::vector<Node *>, MinHeapComparator> min_heap;
public:
    HuffmanCompression()
    {
    }
    void Compress(const std::string &input_path)
    {
        // ------------------------------------------------------------------------
        // Break path down
        // ------------------------------------------------------------------------
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
        Node *root = this->min_heap.top();
        this->min_heap.pop();
        PrintTree(root);
        std::string current_code = "";
        GenerateHuffmanCodes(root, current_code, this->codes, this->reverse);

        EncodeData();
        PadData();
        WriteData();

        PrintCodes();
    }
    void Decompress(const std::string &input_path)
    {
        std::ifstream decomp(input_path, std::ios::out | std::ios::binary);
        decomp.unsetf(std::ios::skipws);

        // get its size:
        std::streampos fileSize;

        decomp.seekg(0, std::ios::end);
        fileSize = decomp.tellg();
        decomp.seekg(0, std::ios::beg);

        // reserve capacity
        std::vector<unsigned char> vec;
        vec.reserve(fileSize);

        // read the data:
        vec.insert(vec.begin(),
                std::istream_iterator<unsigned char>(decomp),
                std::istream_iterator<unsigned char>());

        unsigned char pad = vec[0];

        std::string decomp_data = "";
        for (unsigned int i = 0; i < vec.size(); i++){
            unsigned char c = vec[i];
            std::string b = "";
            for (char j = 0; j < 8; j++)
            {
                b.insert(0, (vec[i] & 1 ? "1" : "0"));
                vec[i] >>= 1;
            }
            decomp_data += b;
        }

        std::string encoded = decomp_data;
        encoded.erase(0,8);
        encoded.erase(encoded.size()-pad);
        
        std::string current_code = "";
        std::string decoded = "";
        for (char b : encoded) {
            current_code += b;
            if (this->reverse.find(current_code) != this->reverse.end()) {
                char c = this->reverse[current_code];
                decoded += c;
                current_code = "";
            }
        }

        // write data out
    }
private:
    void ReadData()
    {
        std::ifstream ifile(this->input_path);
        std::stringstream buffer;
        buffer << ifile.rdbuf();
        this->data = buffer.str();
    }
    void MakeFrequencyMap()
    {
        for (const char &c : this->data)
        {
            if (this->frequency_map.find(c) == this->frequency_map.end())
                this->frequency_map[c] = 0;
            this->frequency_map[c] += 1;
        }
    }
    void FillMinHeap()
    {
        for (const std::pair<char, unsigned long> &p : this->frequency_map)
        {
            Node *n = new Node;
            n->ch = p.first;
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
    }
    void GenerateHuffmanCodes(Node *root, const std::string &current_code, std::map<char, std::string> &codes, std::map<std::string, char> &reverse_mapping)
    {
        if (root == nullptr)
            return;

        if (root->ch != '\0')
        {
            codes[root->ch] = current_code;
            reverse_mapping[current_code] = root->ch;
            return;
        }

        GenerateHuffmanCodes(root->left, current_code + "0", codes, reverse_mapping);
        GenerateHuffmanCodes(root->right, current_code + "1", codes, reverse_mapping);
    }
    void PrintCodes() {
        for (std::map<std::string, char>::iterator i = this->reverse.begin(); i != this->reverse.end(); i++) {
            std::cout << i->first << " " << i->second << "\n";
        }
    }
    void EncodeData()
    {
        for (char i : this->data)
            this->encoded_data.append(codes[i]);
    }
    void PadData()
    {
        this->padded_data = this->encoded_data;
        unsigned int pad_length = 8 - (this->padded_data.size() % 8);
        for (unsigned int i = 0; i < pad_length; i++)
            this->padded_data.append("0");
        std::bitset<8> pad_binary(pad_length);
        this->padded_data.insert(0, pad_binary.to_string());
    }
    void WriteData()
    {
        std::ofstream outfile;
        outfile.open(this->output_path, std::ios::out | std::ios::binary);
        for (unsigned int i = 0; i < this->padded_data.size(); i += 8)
        {
            int b = stoi(this->padded_data.substr(i, 8), 0, 2);
            outfile.write(reinterpret_cast<const char *>(&b), 1);
            std::cout << "(" << i + 8 << "/" << this->padded_data.size() << ")\r";
        }
        std::cout << std::endl;
        outfile.close();
    }
};

int main(int argc, char *argv[])
{
    HuffmanCompression hc;
    // hc.Compress("/Users/james/Mahlet/Basic_Concole_App copy/README.md");
    hc.Compress("/Users/james/Downloads/big.txt");
    hc.Decompress("/Users/james/Downloads/big.bin");
    return 0;
}
