#include "DH_dynamic_huffman.h"
#define MAX_DEPTH 200

int already_transmitted(DH_tree *tr, unsigned char c)
{
    if (tr->leaf_node[c] == NULL)
        return 0;
    else
        return 1;
}

//transmitting root to leaf path of existing node
void transmit_symbol_code(DH_tree *tr, queue *q, unsigned char c)
{
    unsigned char reverse_path[MAX_DEPTH]; //used as stack
    int idx = 0;

    //leaf node containing symbol c
    DH_node *p = tr->leaf_node[c];

    while (p != tr->root)
    {
        if (DH_is_left_child(p))
            reverse_path[idx] = 0;
        if (DH_is_right_child(p))
            reverse_path[idx] = 1;
        idx++;
        p = p->parent;
    }

    int i;
    for (i = idx - 1; i >= 0; i--)
        deque_push(q, reverse_path[i]);

}

//transmitting root to leaf path of nyt node
void transmit_nyt_code(DH_tree *tr, queue *q)
{
    unsigned char reverse_path[MAX_DEPTH]; //used as stack
    int idx = 0;

    //leaf node containing nyt
    DH_node *p = tr->nyt;

    while (p != tr->root)
    {
        if (DH_is_left_child(p))
            reverse_path[idx] = 0;
        if (DH_is_right_child(p))
            reverse_path[idx] = 1;
        idx++;
        p = p->parent;
    }


    int i;
    for (i = idx - 1; i >= 0; i--)
        deque_push(q, reverse_path[i]);
}

//pushes ASCII code of c into q, from lsb to msb
void transmit_ASCII_code(deque *q, unsigned char c)
{
    unsigned char mask = 1;
    unsigned char lsb; //least significant bit

    int i;
    for (i = 0; i < CHAR_BIT; i++)
    {
        lsb = c & mask;
        deque_push(q, lsb);
        c >>= 1;
    }
}

//insert new symbol into tree by splitting
//nyt node into inner node, new nyt node and new leaf node
//together with reconnecting them
void DH_insert_symbol(DH_tree *tr, unsigned char c)
{
    DH_node *new_nyt = DH_create_node();
    DH_node *new_leaf = DH_create_node();
    DH_node *old_nyt = tr->nyt;

    old_nyt->right = new_leaf;
    new_leaf->parent = old_nyt;
    new_leaf->val = c;
    tr->leaf_node[c] = new_leaf;

    old_nyt->left = new_nyt;
    new_nyt->parent = old_nyt;
    tr->nyt = new_nyt;
}

void DH_encode(DH_tree *tr, queue *q, unsigned char coded_char)
{
    if (already_transmitted(tr, coded_char))
    {
        transmit_symbol_code(tr, q, coded_char);
    }
    else
    {
        transmit_nyt_code(tr, q);
        transmit_ASCII_code(q, coded_char);
        DH_insert_symbol(tr, coded_char);
    }

    DH_update(tr, coded_char);
}
