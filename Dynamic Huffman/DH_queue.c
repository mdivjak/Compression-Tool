#include "DH_queue.h"

/**
* DEQUE of unsigned char type
*/
struct deque
{
    int capacity;
    int front, rear;
    int size;
    unsigned char *arr;
};

deque *create_deque(int capacity)
{
    deque *q = (deque*) malloc(sizeof(deque));
    q->arr = (unsigned char*) malloc(capacity * sizeof(unsigned char));
    q->front = q->rear = -1;
    q->size = 0;
    q->capacity = capacity;

    return q;
}

int deque_empty(deque *q)
{
    return (q->size == 0);
}

unsigned char deque_front(deque *q)
{
    if (deque_empty(q))
        return -1;
    else
        return q->arr[q->front];
}

void deque_push(deque *q, unsigned char x)
{
    if (deque_empty(q))
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

void deque_pop_front(deque *q)
{
    if (deque_empty(q))
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

int deque_size(deque *q)
{
    return q->size;
}

void delete_deque(deque *q)
{
    free(q->arr);
    free(q);
}

void deque_pop_back(deque *q, int x)
{
    while (x--)
    {
        if (deque_empty(q))
            return;
        else
        {
            if (q->front == q->rear)
                q->front = q->rear = -1;
            else
            {
                if (--q->rear < 0) q->rear += q->capacity;
            }
        }
        q->size--;
    }
}


/**
* QUEUE of DH_node* type
*/
struct queue
{
    int capacity;
    int front, rear;
    int size;
    DH_node **arr;
};

queue *create_queue(int capacity)
{
    queue *q = (queue*) malloc(sizeof(queue));
    q->arr = (DH_node**) malloc(capacity * sizeof(DH_node*));
    q->front = q->rear = -1;
    q->size = 0;
    q->capacity = capacity;

    return q;
}

int queue_empty(queue *q)
{
    return (q->size == 0);
}

DH_node* queue_front(queue *q)
{
    if (queue_empty(q))
        return NULL;
    else
        return q->arr[q->front];
}

void queue_push(queue *q, DH_node* x)
{
    if (queue_empty(q))
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

void queue_pop(queue *q)
{
    if (queue_empty(q))
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

int queue_size(queue *q)
{
    return q->size;
}

void delete_queue(queue *q)
{
    free(q->arr);
    free(q);
}








