#include "DH_dynamic_huffman.h"

//swaps two nodes together with their subtrees
void swap_nodes(DH_node *nd1, DH_node *nd2)
{
    if (DH_is_left_child(nd1))
        nd1->parent->left = nd2;
    else
        nd1->parent->right = nd2;

    DH_node *parent2 = nd2->parent;

    if (DH_is_left_child(nd2))
        parent2->left = nd1;
    else
        parent2->right = nd1;

    nd2->parent = nd1->parent;
    nd1->parent = parent2;
}

//returns pointer to a node with which *changed should be swapped
//if no such node exists, returns NULL
//searches for it using leverl-order traversal of *tr tree
DH_node *find_swap_node(DH_tree *tr, DH_node *changed)
{
    DH_node *curr;
    queue *nq = create_queue(QUEUE_SIZE);
    queue_push(nq, tr->root);


    while (!queue_empty(nq))
    {
        curr = queue_front(nq);
        queue_pop(nq);

        if (curr->right != NULL &&
            curr->right->weight < changed->weight)
            {
                delete_queue(nq);
                return curr->right;
            }

        if (curr->right != NULL &&
            curr->right == changed)
            {
                delete_queue(nq);
                return NULL;
            }


        if (curr->left != NULL &&
            curr->left->weight < changed->weight)
            {
                delete_queue(nq);
                return curr->left;
            }

        if (curr->left != NULL &&
            curr->left == changed)
            {
                delete_queue(nq);
                return NULL;
            }


        if (curr->right != NULL &&
            DH_is_not_leaf(curr->right))
            {
                queue_push(nq, curr->right);
            }

        if (curr->left != NULL &&
            DH_is_not_leaf(curr->left))
            {
                queue_push(nq, curr->left);
            }


    }

    delete_queue(nq);
    return NULL;
}

void DH_update(DH_tree *tr, unsigned char changed_symbol)
{
    DH_node *swap_node;
    DH_node *changed = tr->leaf_node[changed_symbol];

    changed->weight++;
    while (changed != tr->root)
    {
        swap_node = find_swap_node(tr, changed);

        if (swap_node != NULL && swap_node != changed->parent)
        {
            swap_nodes(swap_node, changed);
        }


        changed = changed->parent;
        changed->weight++;
    }
}
