#include "queue.h"

/**
* QUEUE of unsigned char type
*/

queue *DFL_create_queue(int capacity)
{
    queue *q = (queue*) malloc(sizeof(queue));
    q->arr = (unsigned char*) malloc(capacity * sizeof(unsigned char));
    q->front = q->rear = -1;
    q->size = 0;
    q->capacity = capacity;

    return q;
}

int DFL_queue_empty(queue *q)
{
    return (q->size == 0);
}

unsigned char DFL_queue_front(queue *q)
{
    if (DFL_queue_empty(q))
        return -1;
    else
        return q->arr[q->front];
}

void DFL_queue_push(queue *q, unsigned char x)
{
    if (DFL_queue_empty(q))
    {
        q->front = q->rear = 0;
        q->arr[0] = x;
    }
    else
    {
        q->rear = (q->rear + 1) % q->capacity;
        q->arr[q->rear] = x;
    }

    q->size++;
}

void DFL_queue_pop(queue *q)
{
    if (DFL_queue_empty(q))
        return;
    else
    {
        if (q->front == q->rear)
            q->front = q->rear = -1;
        else
            q->front = (q->front + 1) % q->capacity;
    }

    q->size--;
}

int DFL_queue_size(queue *q)
{
    return q->size;
}

void DFL_delete_queue(queue *q)
{
    free(q->arr);
    free(q);
}

int DFL_queue_full(queue *q)
{
    return (q->size == q->capacity);
}

void DFL_queue_clear(queue *q)
{
    q->size = 0;
    q->front = q->rear = -1;
}
