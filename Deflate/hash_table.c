#include "deflate.h"

typedef struct hash_table
{
    hash_node *bucket[HASH_UNIVERSE];
} hash_table;

hash_table *create_hash_table()
{
    hash_table *ht = (hash_table*) malloc(sizeof(hash_table));
    int i;
    for (i = 0; i < HASH_UNIVERSE; i++)
        ht->bucket[i] = NULL;
    return ht;
}

hash_node *create_hash_node()
{
    hash_node *hn = (hash_node*) malloc(sizeof(hash_node));
    return hn;
}

void hash_insert(hash_table *ht, int hash_key, int hash_val)
{
    hash_node *new_node = create_hash_node();
    new_node->hash_val = hash_val;
    new_node->next = ht->bucket[hash_key];
    ht->bucket[hash_key] = new_node;
}

hash_node *search_hash(hash_table *ht, int hash_key)
{
    return ht->bucket[hash_key];
}

void delete_hash_table(hash_table *ht)
{
    hash_node *head, *prev;
    int i;
    for (i = 0; i < HASH_UNIVERSE; i++)
    {
        prev = NULL;
        head = ht->bucket[i];
        while (head != NULL)
        {
            prev = head;
            head = head->next;
            free(prev);
        }
    }

    free(ht);
}
