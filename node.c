#include "node.h"

#include <stdio.h>
#include <stdlib.h>

Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    n->symbol = symbol;
    n->frequency = frequency;
    n->left = NULL;
    n->right = NULL;
    return n;
}

void node_delete(Node **n) {
    if (*n) {
        free(*n);
        *n = NULL;
    }
}

Node *node_join(Node *left, Node *right) {
    Node *parent = node_create(36, (left->frequency + right->frequency)); //creates new parent node
    parent->left = left;
    parent->right = right;
    return parent;
}
void node_print(Node *n) {
    printf("frequency = %lu\n", n->frequency);
    printf("symbol = %hhu\n", n->symbol);
}
