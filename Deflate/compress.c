#include "deflate.h"


//groups bits from queue into bytes and output them
void output_from_queue(queue *q_out, FILE *out)
{
    unsigned char output1;
    unsigned char mask = 0x80;
    int i;

    while (DFL_queue_size(q_out) >= 8)
    {
        output1 = 0;
        for (i = 0; i < CHAR_BIT; i++)
        {
            output1 |= DFL_queue_front(q_out);
            DFL_queue_pop(q_out);

            output1 <<= (i < 7);
        }

        //printf("output:"BYTE_TO_BINARY_PATTERN"    %d\n", BYTE_TO_BINARY(output1), output1);
        fputc(output1, out);
    }
}

//checks if whole input file is read before running into EOF
int all_read(int read, FILE *in)
{
    if (read < BLOCK_SIZE || feof(in))
        return 1;
}

//having queue of size less than 8 it
//adds zeros to form a whole byte and prints it out
void flush_queue(queue *q_out, FILE *out)
{
    if (DFL_queue_empty(q_out))
        return;

    while (DFL_queue_size(q_out) < 8)
        DFL_queue_push(q_out, 0);

    output_from_queue(q_out, out);
}

//pushed symbol into queue bitwise
//num_of_bits is number of bits being pushed (will always fit symbol value)
//msb going first into queue
void DFL_output(int num_of_bits, unsigned short symbol, queue *q_out)
{
    int i;
    for (i = 0; i < num_of_bits; i++)
    {
        DFL_queue_push(q_out, (symbol & (1 << (num_of_bits - 1))) !=  0);
        symbol <<= 1;
    }
}

void output_from_both_queues(queue *q_out, queue *sh_compressed, FILE *out)
{
    while (DFL_queue_size(q_out) % 8 != 0)
    {
        DFL_queue_push(q_out, DFL_queue_front(sh_compressed));
        DFL_queue_pop(sh_compressed);
    }

    output_from_queue(q_out, out);
    output_from_queue(sh_compressed, out);

    while (!DFL_queue_empty(sh_compressed))
    {
        DFL_queue_push(q_out, DFL_queue_front(sh_compressed));
        DFL_queue_pop(sh_compressed);
    }
}

int DFL_compress(FILE *in, FILE *out)
{
    fputc(3, out); //for general decoder; 3 is deflate code

    hash_table *ht = create_hash_table();
    queue *q_out = DFL_create_queue(1000); //random buffer size; probably would fit into smaller
    queue *lz77_buffer = DFL_create_queue(BLOCK_SIZE / 2); //sliding window of last 32KB

    unsigned char block[BLOCK_SIZE];
    int read;

    queue *sh_compressed = DFL_create_queue(BLOCK_SIZE * 8);
    int sh_sz;

    unsigned char first_byte;

    int block_num = 0;
    while ((read = fread(block, CHAR_BYTE, BLOCK_SIZE, in)) > 0)
    {
        first_byte = 0;

        if (all_read(read, in))
            first_byte |= 0x80;

        sh_sz = compress_sh(block, read, block_num, lz77_buffer, sh_compressed, ht);

        if (sh_sz == -1) //when compressed is bigger than non-compressed
        {
            DFL_output(8, first_byte, q_out);
            output_from_queue(q_out, out);
            int i;
            for (i = 0; i < read; i++)
            {
                add_to_buffer(lz77_buffer, block[i]);
                fputc(block[i], out);
                if (i < read - 2)
                    feed_hash(block[i], block[i + 1], block[i + 2], block_num, i, ht);
            }
            first_byte = 0;
            DFL_output(8, first_byte, q_out);
            output_from_queue(q_out, out);  ///izmenjeno
        }
        else
        {
            first_byte |= 0x20;
            DFL_output(8, first_byte, q_out);
            output_from_both_queues(q_out, sh_compressed, out);
            first_byte = 0;
            DFL_output(7, first_byte, q_out);
            output_from_queue(q_out, out);
            flush_queue(q_out, out);
        }

        block_num++;
    }

    flush_queue(q_out, out);
    DFL_delete_queue(q_out);
    DFL_delete_queue(sh_compressed);
    DFL_delete_queue(lz77_buffer);
    delete_hash_table(ht);

    return 1;
}
