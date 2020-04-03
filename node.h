#pragma once

#include <ostream>

struct Node
{
    char *ch = nullptr;
    unsigned long frequency = 0;
    Node *left = nullptr;
    Node *right = nullptr;
    friend std::ostream &operator<<(std::ostream &os, const Node &n)
    {
        os << (*(n.ch) != '\n' && *(n.ch) != '\0' ? std::string(1, *(n.ch)) : "") << (*(n.ch) != '\0' ? "," : "") << n.frequency;
        return os;
    }
};

class MinHeapNodeComparator
{
public:
    int operator()(Node *p1, Node *p2);
};

void delete_node(Node *node);
