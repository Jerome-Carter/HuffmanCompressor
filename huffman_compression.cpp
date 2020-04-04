#include "huffman_compression.h"

HuffmanCompression::HuffmanCompression()
    : tree(nullptr), read_data(nullptr)
{
}

HuffmanCompression::~HuffmanCompression()
{
    delete_node(tree);
    delete read_data;
}

void HuffmanCompression::Compress(const std::string &input_path)
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

void HuffmanCompression::Decompress(const std::string &input_path, const std::string &output_path)
{
    this->input_path = input_path;
    this->output_path = output_path;
    ReadData();
    DecodeData();
    WriteData();
}

void HuffmanCompression::ReadData()
{
    if (this->read_data != nullptr)
        this->read_data->clear();
    std::ifstream ifile(this->input_path, std::ios::in | std::ios::binary);
    this->read_data = new std::vector<char>(std::istreambuf_iterator<char>(ifile), {});
}

void HuffmanCompression::DecodeData()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<char> extracted = *(this->read_data);
    this->pad_bits = extracted.at(0);
    extracted.erase(extracted.begin());

    this->write_data = "";
    std::string current_code = "";
    for (char b : extracted)
    {
        for (char i = 0; i < 8; i++)
        {
            current_code += ((b >> (7 - i)) & 1 ? "1" : "0");
            if (this->reverse.count(current_code))
            {
                this->write_data += std::bitset<8>(this->reverse[current_code]).to_string();
                current_code = "";
            }
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Decoding data took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
              << " milliseconds\n";
}

void HuffmanCompression::MakeFrequencyMap()
{
    this->frequency_map.clear();
    // Changing this made great time improvements
    // There may be more optimization possible here
    for (const char &c : *(this->read_data))
        this->frequency_map[c]++;
}

void HuffmanCompression::FillMinHeap()
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

void HuffmanCompression::GenerateHuffmanTree()
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

void HuffmanCompression::GenerateHuffmanCodes(Node *root, const std::string &current_code)
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

void HuffmanCompression::PrintCodes() const
{
    for (std::map<std::string, char>::const_iterator i = this->reverse.begin(); i != this->reverse.end(); i++)
    {
        std::cout << i->first << " " << std::bitset<8>(i->second).to_string() << "\n";
    }
}

void HuffmanCompression::EncodeData()
{
    for (char i : *(this->read_data))
        this->encoded_data.append(codes[i]);
}

void HuffmanCompression::PadData()
{
    this->write_data = this->encoded_data;
    unsigned int pad_length = 8 - (this->write_data.size() % 8);
    this->write_data.append(pad_length, '0');
    std::bitset<8> pad_binary(pad_length);
    this->write_data.insert(0, pad_binary.to_string());
}

void HuffmanCompression::WriteData() const
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