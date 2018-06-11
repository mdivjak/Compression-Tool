#include "DH_dynamic_huffman.h"
#define OUTPUT_SIZE 16384

//pushes c into q bit by bit, from lsb to msb
void push_bit_by_bit(deque *q, char c)
{
    int i;
    for (i = 0; i < CHAR_BIT; i++)
    {
        deque_push(q, c & 1);
        c >>= 1;
    }
}

//decodes path to leaf node (symbol node or nyt)
DH_node *decode_to_leaf(DH_tree *tr, deque *q, FILE *in, int x)
{
    //no bits left to decompress
    if (deque_empty(q) && feof(in))
        return NULL;

    char c;
    unsigned char dir;
    DH_node *curr = tr->root;
    while (DH_is_not_leaf(curr))
    {
        if (deque_size(q) <= 8 && !feof(in))
        {
            char c = fgetc(in);

            if (!feof(in))
                push_bit_by_bit(q, c);
            else
                deque_pop_back(q, x);
        }

        dir = deque_front(q);
        deque_pop_front(q);

        if (dir == 1)
            curr = curr->right;
        else
            curr = curr->left;
    }

    return curr;
}

char new_symbol(FILE *in, deque *q, int x)
{
    unsigned char s = 0;
    int i;

    unsigned char mask = 0x80;

    if (deque_empty(q) && feof(in))
        return 0;

    for (i = 0; i < CHAR_BIT; i++)
    {
        if (deque_size(q) <= 8 && !feof(in))
        {
            char c = fgetc(in);

            if (!feof(in))
                push_bit_by_bit(q, c);
            else
                deque_pop_back(q, x);
        }

        if (deque_front(q))
            s |= mask;
        deque_pop_front(q);

        if (i != 7)
            s >>= 1;
    }

    return s;
}


int DH_decompress(FILE *in, FILE *out)
{
    char output[OUTPUT_SIZE];
    int idx = 0;

    char x = fgetc(in);

    DH_tree *tr = DH_create_tree();
    deque *q = create_deque(DEQUE_SIZE);

    DH_node *leaf;
    char changed_symbol;
    while (1)
    {
        leaf = decode_to_leaf(tr, q, in, x);

        if (leaf == NULL)
            break;

        if (leaf == tr->nyt)
        {
            changed_symbol = new_symbol(in, q, x);
            DH_insert_symbol(tr, changed_symbol);
        }
        else
        {
            changed_symbol = leaf->val;
        }

        output[idx++] = changed_symbol;
        if (idx == OUTPUT_SIZE)
        {
            fwrite(output, 1, OUTPUT_SIZE, out);
            idx = 0;
        }
        DH_update(tr, changed_symbol);
    }
    fwrite(output, 1, idx, out);


    delete_deque(q);
    DH_delete_tree(tr);
    return 1;
}
