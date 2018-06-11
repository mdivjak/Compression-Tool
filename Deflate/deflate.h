#ifndef DEFLATE_H_INCLUDED
#define DEFLATE_H_INCLUDED
#include <stdio.h>
#include "queue.h"
#define HASH_UNIVERSE 100000
#define BLOCK_SIZE 65536
#define CHAR_BYTE 1
#define CHAR_BIT 8

/**
* @struct hash_node Will be used for storing link listed as hash table is implemented with open hashing.
*/
typedef struct hash_node
{
    int hash_val;
    struct hash_node *next;
} hash_node;

/**
* @struct hash_table Open hashing/closed addressing hash table.
*/
typedef struct hash_table hash_table;

/**
* @brief Function which allocates space for hash_table and returns pointer to it.
* @return hash_table* Pointer to a created hash_table.
*/
hash_table *create_hash_table();

/**
* @brief Function which feeds data to hash table.
* @param[in] hash_table* Pointer to a hash_table which will be updated.
* @param[in] int hash_key Computed hash key of an object which is being inserted.
* @param[in] int hash_value Computed hash value of an object which is being inserted.
*/
void hash_insert(hash_table *ht, int hash_key, int hash_val);

/**
* @brief Function which searches through data in hash_table.
* @param[in] hash_table* Pointer to a hash_table which is being searched.
* @param[in] int hash_key Target hash key.
* @return hash_node* Head of a singly-linked list containing all objects with desired hash key.
*/
hash_node *search_hash(hash_table *ht, int hash_key);

/**
* @brief Function which deletes hash_table and allocates its memory.
* @param[in] hash_table* Pointer to a hash_table which is being deleted.
*/
void delete_hash_table(hash_table *ht);

/**
* @brief Function which has more proper problem-specific interface for updating hash table than hash_insert.
* @param[in] unsigned char byte1 First byte of sequence searched for.
* @param[in] unsigned char byte2 Second byte of sequence searched for.
* @param[in] unsigned char byte3 Third byte of sequence searched for.
* @param[in] int block_num Number of block being read and compressed. Needed for computing distance for lz77.
* @param[in] int block_idx Byte index of current block. Needed for computing distance for lz77.
* @param[in] hash_table* Pointer to a hash_table which is being deleted.
*/
void feed_hash(unsigned char byte1, unsigned char byte2, unsigned char byte3, int block_num, int block_idx, hash_table *ht);

/**
* @struct queue Heavily used throughout program. Primarily used as buffer containing sliding window for lz77 and
* output buffer resolving not-whole-bytes output with Huffman coding. Implemented sequentally.
*/
typedef struct queue queue;

/**
* @brief Moves sliding window for lz77 algorithm with push/pop operations.
* @param[in] queue* Queue representing sliding window.
* @param[in] unsigned char val Symbol we are pushing into buffer.
*/
void add_to_buffer(queue *buffer, unsigned char val);

/**
* @brief Function which compresses contents of one file and writes it into other.
* @param[in] FILE *in Pointer to a file from which original data is being read.
* @param[in] FILE *out Pointer to a file in which compressed data will be written.
*/
int DFL_compress(FILE *in, FILE *out);

/**
* @brief Function which compresses data block using lz77 and static Huffman coding.
* @param[in] unsigned char block[] Block of data that is to be compressed.
* @param[in] int block_size Size, represented in bytes, of block.
* @param[in] queue *lz77_buffer Sliding window containing last 32KB of last elements.
* @param[in] hash_table *ht Pointer to a hash_table which pairs with lz77_buffer
*/
int compress_sh(unsigned char block[], int block_size, int block_num, queue *lz77_buffer, queue *sh_out, hash_table *ht);

/**
* @brief Function which pushes bits into queue to form a whole byte which can then be outputted.
* @param[in] int num_of_bits Number of bits a number should be pushed as.
* @param[in] unsigned shot symbol Number being pushed into queue.
* @param[in] queue *q_out queue serving as output buffer
*/
void DFL_output(int num_of_bits, unsigned short symbol, queue *q_out);

/**
* @brief Function which decompresses contents of one file and writes it into other.
* @param[in] FILE *in Pointer to a file from which compressed data is being read.
* @param[in] FILE *out Pointer to a file in which uncompressed data will be written.
*/
int DFL_decompress(FILE *in, FILE *out);
#endif // DEFLATE_H_INCLUDED
