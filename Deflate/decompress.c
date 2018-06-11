#include "deflate.h"
#include <stdlib.h>

//loads new byte into queue and pushes it as bits, msb first
void load_queue(FILE *in, queue *q)
{
    if (feof(in))
    {
        printf("Error: Trying to read from an empty file.\n");
        exit(1);
    }

    unsigned char c = fgetc(in);

    int i;
    for (i = 0; i < CHAR_BIT; i++)
    {
        DFL_queue_push(q, ((c & 0x80) != 0));
        c <<= 1;
    }
}

//finds whether it is last_block or not
//finds compression method of following block
//discards rest of overhead byte
void read_block_overhead(FILE *in, queue *q_in, int *last_block, int *comp_type)
{
    load_queue(in, q_in);

    *last_block = DFL_queue_front(q_in);
    DFL_queue_pop(q_in);
    DFL_queue_pop(q_in);
    *comp_type = DFL_queue_front(q_in);

    DFL_queue_clear(q_in);
}

//reads num_of_bits from queue
unsigned short read_bits(FILE *in, queue* q_in, int num_of_bits)
{
    if (num_of_bits > 13)
    {
        printf("Error in function read_bits.\n");
        system("pause");
    }

    unsigned short ret = 0;
    int i;
    for (i = 0; i < num_of_bits; i++)
    {
        if (DFL_queue_empty(q_in))
            load_queue(in, q_in);

        ret |= DFL_queue_front(q_in);
        DFL_queue_pop(q_in);

        ret <<= (i < num_of_bits - 1);
    }

    return ret;
}

//decodes back reference length
int extract_length(FILE *in, queue *q_in, int len_code)
{
    if (len_code > 285 || len_code < 257)
    {
        printf("Error in function extract_length: %d.\n", len_code);
        system("pause");
    }

    len_code -= 257;
    int len_extra_bits[29] = {0};
    int i;
    int curr = 0;
    for (i = 8; i < 28; i++)
    {
        if (i % 4 == 0)
            curr++;
        len_extra_bits[i] = curr;
    }

    int len_start_val[29] = {0};
    for (i = 0; i < 8; i++)
        len_start_val[i] = i + 3;
    for (i = 8; i < 29; i++)
        len_start_val[i] = len_start_val[i - 1] + (1 << len_extra_bits[i - 1]);
    len_start_val[28]--;

    int extra_bits = len_extra_bits[len_code];
    int extra_val = read_bits(in, q_in, extra_bits);

    return len_start_val[len_code] + extra_val;
}

//decodes back reference distance
int extract_distance(FILE *in, queue *q_in)
{
    int dist_extra_bit[30] = {0};
    int curr = 0;
    int i;
    for (i = 4; i < 30; i++)
    {
        if (i % 2 == 0)
            curr++;
        dist_extra_bit[i] = curr;
    }

    int dist_start_val[30] = {0};
    for (i = 0; i < 4; i++)
        dist_start_val[i] = i + 1;
    for (i = 4; i < 30; i++)
        dist_start_val[i] = dist_start_val[i - 1] + (1 << dist_extra_bit[i - 1]);

    int dist_code = read_bits(in, q_in, 5);

    if (dist_code >= 30)
    {
        printf("Error in function extract_distance.\n");
        system("pause");
    }

    int extra_bits = dist_extra_bit[dist_code];
    int extra_val = read_bits(in, q_in, extra_bits);

    return dist_start_val[dist_code] + extra_val;
}


//decodes back reference and outputs it
void print_reference(FILE *in, FILE *out, queue *q_in, unsigned short len_code, queue *buffer)
{
    int length = extract_length(in, q_in, len_code);
    int distance = extract_distance(in, q_in);

    int pos = buffer->rear - distance + 1;
    if (pos < 0) pos += buffer->capacity;
    int i;
    for (i = 0; i < length; i++)
    {
        fputc(buffer->arr[pos], out);
        add_to_buffer(buffer, buffer->arr[pos]);
        pos = (pos + 1) % buffer->capacity;
    }
}

//decodes symbol according to deflate static Huffman algorithm alphabet provided by RFC 1951
int decode_symbol(FILE *in, FILE *out, queue *q_in, queue *buffer)
{
    unsigned short symbol = 0;
    symbol = read_bits(in, q_in, 5);

    int categorie = (symbol & 0x0018) >> 3;

    if (categorie == 1 || categorie == 2)
    {
        unsigned short extra_val = read_bits(in, q_in, 3);
        symbol = (symbol << 3) | extra_val;

        symbol -= 48;
        fputc(symbol, out);
        add_to_buffer(buffer, symbol);
    }
    else if (categorie == 0)
    {
        int subcategorie = (symbol & 0x0006) >> 1;
        if (subcategorie == 3)
        {
            unsigned short extra_val = read_bits(in, q_in, 3);
            symbol = (symbol << 3) | extra_val;

            symbol -= 48;
            fputc(symbol, out);
            add_to_buffer(buffer, symbol);
        }
        else
        {
            unsigned short extra_val = read_bits(in, q_in, 2);

            symbol = (symbol << 2) | extra_val;

            symbol += 256;

            if (symbol == 256)
                return 1;

            print_reference(in, out, q_in, symbol, buffer);
        }
    }
    else if (categorie == 3)
    {
        int subcategorie = symbol & 0x0007;
        if (subcategorie == 0)
        {
            unsigned short extra_val = read_bits(in, q_in, 3);
            symbol = (symbol << 3) | extra_val;

            symbol += 88;

            print_reference(in, out, q_in, symbol, buffer);
        }
        else
        {
            unsigned short extra_val = read_bits(in, q_in, 4);
            symbol = (symbol << 4) | extra_val;

            symbol -= 256;
            fputc(symbol, out);
            add_to_buffer(buffer, symbol);
        }
    }

    return 0;
}

int decode_block(FILE *in, FILE *out, queue *q_in, queue *buffer)
{
    int last_block, comp_type;
    read_block_overhead(in, q_in, &last_block, &comp_type);

    if (comp_type == 0)
    {
        char block[BLOCK_SIZE];
        int read = fread(block, CHAR_BYTE, BLOCK_SIZE, in);
        if (read == BLOCK_SIZE)
        {
            int i;
            for (i = 0; i < read; i++)
            {
                add_to_buffer(buffer, block[i]);
                fputc(block[i], out);
            }
            fgetc(in);
        }
        else
        {
            int i;
            for (i = 0; i < read - 1; i++)
            {
                add_to_buffer(buffer, block[i]);
                fputc(block[i], out);
            }
        }
    }
    else if (comp_type == 1)
    {
        int end_of_block = 0;
        while (!end_of_block)
        {
            end_of_block = decode_symbol(in, out, q_in, buffer);
        }

    }

    return last_block;
}

int DFL_decompress(FILE *in, FILE *out)
{
   // fgetc(in);

    queue *q_in = DFL_create_queue(1000);
    queue *buffer = DFL_create_queue(BLOCK_SIZE / 2);

    int last_block = 0;
    while (!last_block)
    {
        DFL_queue_clear(q_in);
        last_block = decode_block(in, out, q_in, buffer);
    }


    DFL_delete_queue(q_in);
    DFL_delete_queue(buffer);
    return 1;
}
