#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

typedef struct queue
{
	int capacity;
	int front, rear;
	int size;
	unsigned char *arr;
} queue;

queue *DFL_create_queue(int capacity);
void DFL_queue_push(queue *q, unsigned char val);
unsigned char DFL_queue_front(queue *q);
void DFL_queue_pop(queue *q);
int DFL_queue_empty(queue *q);
int DFL_queue_size(queue *q);
void DFL_delete_queue(queue* q);
int DFL_queue_full(queue* q);
void DFL_queue_clear(queue *q);

#endif // QUEUE_H_INCLUDED
