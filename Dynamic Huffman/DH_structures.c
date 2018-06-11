#include "DH_dynamic_huffman.h"

DH_node *DH_create_node()
{
    DH_node *new_node = (DH_node*) malloc(sizeof(DH_node));
    new_node->weight = 0;

    new_node->parent =
    new_node->left =
    new_node->right = NULL;

    return new_node;
}

DH_tree *DH_create_tree()
{
    DH_tree *tr = (DH_tree*) malloc(sizeof(DH_tree));
    tr->root = DH_create_node();
    tr->nyt = tr->root;

    int i;
    for (i = 0; i < SYMBOL; i++)
        tr->leaf_node[i] = NULL;

    return tr;
}

void delete_tree_structure(DH_node *nd)
{
    if (nd == NULL)
        return;

    delete_tree_structure(nd->left);
    delete_tree_structure(nd->right);

    free(nd);
}

void DH_delete_tree(DH_tree *tr)
{
    delete_tree_structure(tr->root);
    free(tr);
}

int DH_is_not_leaf(DH_node *nd)
{
    if (nd->left == NULL && nd->right == NULL)
        return 0;
    else
        return 1;
}

int DH_is_left_child(DH_node *nd)
{
    if (nd->parent == NULL)
        return 0;

    if (nd->parent->left == NULL)
        return 0;

    if (nd->parent->left == nd)
        return 1;

    return 0;
}

int DH_is_right_child(DH_node *nd)
{
    if (nd->parent == NULL)
        return 0;

    if (nd->parent->right == NULL)
        return 0;

    if (nd->parent->right == nd)
        return 1;

    return 0;
}
