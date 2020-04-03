#include "node.h"

int MinHeapNodeComparator::operator()(Node *p1, Node *p2)
{
    return p1->frequency > p2->frequency;
}

void delete_node(Node* node)
{
    if (node == nullptr) return;
    delete_node(node->left); 
    delete_node(node->right);
    delete node->ch;
    delete node;  
}
