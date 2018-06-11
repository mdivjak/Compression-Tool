#include "DH_dynamic_huffman.h"
#define BUFFER_SIZE 16384

//output front bits of deque *q into output file
void pop_to_output(deque *q, FILE *out)
{
    unsigned char output;
    unsigned char mask = 0x80;
    int i;

    while (deque_size(q) >= 8)
    {
        output = 0;
        for (i = 0; i < CHAR_BIT; i++)
        {
            if (deque_front((q)) == 1)
                output |= mask;
            deque_pop_front(q);

            output >>= (i < 7);
        }

        fputc(output, out);
    }
}

int DH_compress(FILE *in, FILE *out)
{
    //for general compress function
    //0 is adaptive Huffman code
    fputc(0, out);

    //placeholder; will later be update to
    //number of redundant trailing bits
    fputc(0, out);

    DH_tree *tr = DH_create_tree();
    deque *q = create_deque(DEQUE_SIZE);

    char buffer[BUFFER_SIZE];
    size_t n;
    size_t i;

    while ( (n = fread(buffer, 1, BUFFER_SIZE , in)) > 0)
    {
        for (i = 0; i < n; i++)
        {
            DH_encode(tr, q, buffer[i]);
            pop_to_output(q, out);
        }
    }

    //if compressed file has hanging bits
    //that cannot be grouped into byte
    if (!deque_empty(q))
    {
        //number of redundant bits
        int redundant_bits = 8 - deque_size(q);

        while (deque_size(q) < 8)
            deque_push(q, 0);
        pop_to_output(q, out);

        //sets first byte of compressed file to
        //number of redundant trailing bits
        char first_byte = 0;
        if (redundant_bits > 3)
            first_byte |= 0x04;
        if (redundant_bits == 2 || redundant_bits == 3 || redundant_bits == 6 || redundant_bits == 7)
            first_byte |= 0x02;
        if (redundant_bits % 2 == 1)
            first_byte |= 0x01;

        //updates first byte
        fseek(out, 1, SEEK_SET);
        fputc(first_byte, out);
    }


    delete_deque(q);
    DH_delete_tree(tr);

    return 1;
}
