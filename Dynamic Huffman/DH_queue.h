#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED
#include "DH_dynamic_huffman.h"

typedef struct DH_node DH_node;

typedef struct deque deque;
deque *create_deque(int capacity);
void deque_push(deque *q, unsigned char val);
unsigned char deque_front(deque *q);
void deque_pop(deque *q);
int deque_empty(deque *q);
int deque_size(deque *q);
void deque_pop_back(deque *q, int x);
void delete_deque(deque* q);

typedef struct queue queue;
queue *create_queue(int capacity);
void queue_push(queue *q, DH_node* val);
DH_node* queue_front(queue *q);
void queue_pop(queue *q);
int queue_empty(queue *q);
int queue_size(queue *q);
void delete_queue(queue* q);

#endif // QUEUE_H_INCLUDED
