#include "deflate.h"


typedef struct reference
{
    int len, dist;
} reference;

void add_to_buffer(queue *buffer, unsigned char val)
{
    if (DFL_queue_full(buffer))
        DFL_queue_pop(buffer);
    DFL_queue_push(buffer, val);
}

//measures length of back reference
//hash table makes sure it's at least 3
int measure_length(char block[], int block_size, int block_idx, queue *lz77_buffer, int pos_in_queue)
{
    int cnt = 0;
    pos_in_queue = (lz77_buffer->rear - pos_in_queue + 1);
    if (pos_in_queue < 0) pos_in_queue += lz77_buffer->capacity;

    while (block_idx < block_size)
    {
        if(block[block_idx] == lz77_buffer->arr[pos_in_queue])
            cnt++;
        else
            break;


        block_idx++;

        if (pos_in_queue == lz77_buffer->rear)
        {
            pos_in_queue = block_idx - cnt + 1;
            while (block_idx < block_size)
            {
                if (block[block_idx] == block[pos_in_queue])
                    cnt++;
                else
                    break;

                block_idx++;
                pos_in_queue++;

                if (cnt == 258)
                    return cnt;
            }
        }

        pos_in_queue = (pos_in_queue + 1) % lz77_buffer->capacity;

        if (cnt == 258)
            return cnt;
    }


    if (cnt < 3)
        return -1;

    return cnt;
}

//finds first back reference with length of more than 150 (good enough)
//compromise between speed and compression ratio
reference find_best_reference(char block[], int block_size, queue *lz77_buffer, unsigned char byte1, unsigned char byte2, unsigned char byte3, int block_num, int block_idx, hash_table *ht)
{
    reference best_ref;
    best_ref.len = 0; best_ref.dist = -1;

    int curr_pos = BLOCK_SIZE * block_num + block_idx;
    int min_position = curr_pos - BLOCK_SIZE/2 + 1;

    int hash_key = 0;

    unsigned char *hv_ptr = &hash_key;
    *hv_ptr = byte1; hv_ptr++;
    *hv_ptr = byte2; hv_ptr++;
    *hv_ptr = byte3;

    hash_key %= HASH_UNIVERSE;

    hash_node *hash_chain = search_hash(ht, hash_key);
    int length = 0;
    while (hash_chain != NULL && hash_chain->hash_val >= min_position)
    {
        length = measure_length(block, block_size, block_idx, lz77_buffer, curr_pos - hash_chain->hash_val);

        if (length > best_ref.len)
        {
            best_ref.len = length;
            best_ref.dist = curr_pos - hash_chain->hash_val;
        }
        hash_chain = hash_chain->next;

        if (length > 150)
            break;
    }

    return best_ref;
}

//output static huffman code of symbol
int output_symbol_code(unsigned char symbol, queue *q_out)
{
    if (symbol <= 143)
    {
        DFL_output(8, symbol + 48, q_out);
        return 8;
    }
    else
    {
        unsigned short s = (unsigned short) symbol;
        DFL_output(9, s + 256, q_out);
        return 9;
    }
}

//output static huffman code back reference length
int output_length(int len, queue *q_out)
{
    if (len < 3)
    {
        printf("Error in function output_length.\n");
        exit(1);
    }

    if (len <= 6)
    {
        DFL_output(7, len - 2, q_out);  ///IZMENJENO
        return 7;
    }

    if (len == 258)
    {
        DFL_output(8, 197, q_out);
        return 8;
    }

    int bucket;
    int k, x;
    for (k = 6; k >= 1; k--)
    {
        x = len - 3 - (1 << (k + 1));
        if (x < 0) continue;
        if (x < (1 << (k + 1)))
        {
            bucket = k;
            break;
        }
    }

    int add;
    if (x < (1 << (k - 1))) add = 0;
    else if (x < (1 << k)) add = 1;
    else if (x < (1<<k) + (1 << (k - 1))) add = 2;
    else add = 3;

    int code = (bucket - 1) * 4 + add + 261;

    if (code >= 286  || code <= 260)
    {
        printf("nedozvoljeni kod - kompres.");
        system("pause");
    }

    if (code <= 279)
    {
        DFL_output(7, code - 256, q_out);
        DFL_output(bucket - 1, x, q_out);
        return 7 + bucket - 1;
    }
    else
    {
        DFL_output(8, code - 88, q_out);
        DFL_output(bucket - 1, x, q_out);
        return 8 + bucket - 1;
    }
}

//output static huffman code back reference distance
int output_distance(int dist, queue *q_out)
{
    if (dist <= 2)
    {
        DFL_output(5, dist - 1, q_out);
        return 5;
    }

    int bucket;
    int k, x;
    for (k = 14; k >= 1; k--)
    {
        x = dist  - 1 - (1 << k);
        if (x < 0) continue;
        if (x < (1 << k))
        {
            bucket = k;
            break;
        }
    }

    int code;

    if (x >= (1 << (bucket - 1)))
        code = bucket * 2 + 1;
    else
        code = bucket * 2;

    if (code >= 30)
    {
        printf("Error in function output_distance.\n");
        exit(1);
    }

    DFL_output(5, code, q_out);
    DFL_output(bucket - 1, x, q_out);

    return 5 + bucket - 1;
}

//output static huffman code back reference
int output_back_reference(reference ref, queue *q_out)
{
    int bit_cnt = 0;
    bit_cnt += output_length(ref.len, q_out);
    bit_cnt += output_distance(ref.dist, q_out);
    return bit_cnt;
}

void feed_hash(unsigned char byte1, unsigned char byte2, unsigned char byte3, int block_num, int block_idx, hash_table *ht)
{
    int hash_key = 0;

    unsigned char *hv_ptr = &hash_key;
    *hv_ptr = byte1; hv_ptr++;
    *hv_ptr = byte2; hv_ptr++;
    *hv_ptr = byte3;

    hash_key %= HASH_UNIVERSE;

    int hash_val = BLOCK_SIZE * block_num + block_idx;

    hash_insert(ht, hash_key, hash_val);
}

int compress_sh(unsigned char block[], int block_size, int block_num, queue *lz77_buffer, queue *sh_out, hash_table *ht)
{
    DFL_queue_clear(sh_out);
    int bit_cnt = 0;

    reference best_ref;

    int i;
    for (i = 0; i < block_size; i++)
    {
        if (i < block_size - 3)
        {
            best_ref = find_best_reference(block, block_size, lz77_buffer, block[i], block[i + 1], block[i + 2], block_num, i, ht);

            if (best_ref.len == 0)
            {
                add_to_buffer(lz77_buffer, block[i]);
                bit_cnt += output_symbol_code(block[i], sh_out);
                feed_hash(block[i], block[i + 1], block[i + 2], block_num, i, ht);
            }
            else
            {
                bit_cnt += output_back_reference(best_ref, sh_out);
                int big_i = i + best_ref.len - 2;
                for (; i <= big_i; i++)
                {
                    add_to_buffer(lz77_buffer, block[i]);
                    if (i < block_size - 2)
                        feed_hash(block[i], block[i + 1], block[i + 2], block_num, i, ht);
                }
            }
        }
        else
        {
            bit_cnt += output_symbol_code(block[i], sh_out);
            add_to_buffer(lz77_buffer, block[i]);
        }

        if (bit_cnt >= block_size * 8)
            break;
    }


    if (bit_cnt >= block_size * 8)
    {
        for (; i < block_size; i++)
            add_to_buffer(lz77_buffer, block[i]);
        return -1;
    }

    return bit_cnt/8;
}


