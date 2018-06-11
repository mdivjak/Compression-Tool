#ifndef DYNAMIC_HUFFMAN_H_INCLUDED
#define DYNAMIC_HUFFMAN_H_INCLUDED
#include "DH_queue.h"
#include <stdio.h>
#include <stdlib.h>
#define SYMBOL 256
#define CHAR_BIT 8
#define DEQUE_SIZE 50
#define QUEUE_SIZE 512

typedef struct DH_node
{
    unsigned char val;
    int weight;

    struct DH_node *parent, *left, *right;
} DH_node;

typedef struct DH_tree
{
    DH_node *root;
    DH_node *nyt;
    DH_node *leaf_node[SYMBOL];
} DH_tree;


/**
* @brief Function which allocates memory and creates
* node used to create a Huffman Tree. It sets weight to 0
* and parent, left, right pointers to NULL.
* @return node* It returns pointer to a created node.
*/
DH_node *DH_create_node();

/**
* @brief Function which allocates memory and creates a
* Huffman Tree with just nyt node in it.
* @return tree* It returns pointer to a created tree.
*/
DH_tree *DH_create_tree();

/**
* @brief Function which allocates memory and creates
* Huffman Tree with just nyt node in it.
* @param[in] tree* Pointer to a Huffman Tree that is to be deleted.
*/
void DH_delete_tree(DH_tree *tr);

/**
* @brief Function which checks whether a node is
* left child of its parent.
* @param[in] node* Pointer to a node for which we are checking this condition.
* @return int Returns 1 if node is left child of its parent, 0 if it is right child or has no parent.
*/
int DH_is_left_child(DH_node *nd);

/**
* @brief Function which checks whether a node is
* right child of its parent.
* @param[in] node* Pointer to a node for which we are checking this condition.
* @return int Returns 1 if node is right child of its parent, 0 if it is left child or has no parent.
*/
int DH_is_right_child(DH_node *nd);

/**
* @brief Function which checks whether a node is
* leaf.
* @param[in] node* Pointer to a node for which we are checking this condition.
* @return int Returns 1 if node is a leaf, 0 otherwise.
*/
int DH_is_not_leaf(DH_node *nd);

typedef struct queue queue;

/**
* @brief Function which encodes a single character using already
* existing Huffman Tree. It pushes compressed character into queue.
* @param[in] tr Pointer to an Existing Huffman Tree.
* @param[in] q Pointer to a queue which is used as accumulator
* since we can not output single bits into output file.
* We have to accumulate at least 8 bits in queue before printing them as a character.
* @param[in] c Character to be encoded.
*/
void DH_encode(DH_tree *tr, queue *q, unsigned char c);


/**
* @brief Function which inserts new leaf node with given symbol into Huffman tree.
* @param[in] tr Pointer to an Existing Huffman Tree.
* @param[in] c New symbol to be added to the Tree.
*/
void DH_insert_symbol(DH_tree *tr, unsigned char c);


/**
* @brief Function which updates the Huffman Tree after insertion of
* new symbol or incrementing weight of existing one.
* @param[in] tr Pointer to a Huffman Tree.
* @param[in] changed_symbol Symbol which was last changed. It makes no difference
* whether that symbol is inserted for first time or just its weight is incremented.
*/
void DH_update(DH_tree *tr, unsigned char changed_symbol);


/**
* @brief Function which compresses data from one file and stores in into other.
* It uses Adaptive Huffman algorithm.
* @param[in] in Pointer to a file which contains data to be compressed.
* @param[in] out Pointer to a file in which compressed data shell be stored.
* @return int Returns 1 for successful compression and 0 for unsuccessful.
*/
int DH_compress(FILE *in, FILE *out);

/**
* @brief Function which decompresses data from one file and stores in into other.
* It uses Adaptive Huffman algorithm.
* @param[in] in Pointer to a file which contains data to be decompressed.
* @param[in] out Pointer to a file in which decompressed data shell be stored.
* @return int Returns 1 for successful decompression and 0 for unsuccessful.
*/
int DH_decompress(FILE *in, FILE *out);
#endif // DYNAMIC_HUFFMAN_H_INCLUDED
