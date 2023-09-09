#include "huffman.h"

#include "node.h"
#include "pq.h"
#include "stack.h"

#include <stdbool.h>
#include <stdio.h>

Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *pq = pq_create(ALPHABET);
    for (int i = 0; i < ALPHABET; i += 1) {
        if (hist[i] != 0) { //if the frequency of the value at i is > 0, create a node for it
            Node *new = node_create(i, hist[i]);
            enqueue(pq, new);
        }
    }
    Node *left = NULL;
    Node *right = NULL;
    Node *parent = NULL;
    while (pq_size(pq) > 1) { //dequeues two nodes and creates a parent node unill there is only 1 node left in the queue
        dequeue(pq, &left);
        dequeue(pq, &right);
        parent = node_join(left, right);
        enqueue(pq, parent);
    }
    Node *root = NULL; //dequeues the root node from the priority queue and returns it
    dequeue(pq, &root);
    pq_delete(&pq);
    return root;
}

void build_codes(Node *root, Code table[static ALPHABET]) {
    static Code c = { 0, { 0 } };
    uint8_t bit;
    if (root) {
        if (root->left == NULL && root->right == NULL) {
            table[root->symbol] = c;
        } else {
            code_push_bit(&c, 0); 		//
            build_codes(root->left, table); 	//recursive call of left side of tree
            code_pop_bit(&c, &bit); 		//

            code_push_bit(&c, 1); 		//
            build_codes(root->right, table); 	//recursive call of right side of tree
            code_pop_bit(&c, &bit); 		//
        }
    }
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Stack *s = stack_create(nbytes);
    for (uint16_t i = 0; i < nbytes; i += 1) {
        if (tree[i] == 'L') {
            Node *n = node_create(tree[i + 1], 0); //frequency does not matter for the decoder
            stack_push(s, n);
            i += 1;
        } else {
            Node *right = NULL;
            Node *left = NULL;
            Node *parent = NULL;
            stack_pop(s, &right);
            stack_pop(s, &left);
            parent = node_join(left, right);
            stack_push(s, parent);
        }
    }
    Node *root = NULL;
    stack_pop(s, &root);
    stack_delete(&s);
    return root;
}

void delete_tree(Node **root) {
    if (*root) {
        delete_tree(&(*root)->left);
        delete_tree(&(*root)->right);
        node_delete(root);
        (*root) = NULL;
    }
}
