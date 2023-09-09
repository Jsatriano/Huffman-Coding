#include "pq.h"

#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct PriorityQueue {
    uint32_t capacity;
    uint32_t head;
    uint32_t tail;
    uint32_t size;
    Node **items;
};

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->size = 0;
    q->items = (Node **) calloc(capacity, sizeof(Node *)); //allocates space for all items

    return q;
}

void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->items) {
        free((*q)->items);
        free(*q);
        *q = NULL;
    }
    return;
}

bool pq_empty(PriorityQueue *q) {
    return q->size == 0;
}

bool pq_full(PriorityQueue *q) {
    return q->size == q->capacity;
}

uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

bool enqueue(PriorityQueue *q, Node *n) {
    uint32_t temp = q->tail;
    if (pq_full(q)) {
        return false;
    }
    if (pq_empty(q)) {
        q->items[temp] = n;
        q->size += 1;
        q->tail = (q->tail + 1) % q->capacity;
        return true;
    }
    while (temp != q->head) {
        if (q->items[temp - 1]->frequency > n->frequency) {
            q->items[temp] = q->items[temp - 1];
            temp -= 1;
        } else {
            q->items[temp] = n;
            q->size += 1;
            q->tail = (q->tail + 1) % q->capacity;
            return true;
        }
    }
    if (temp == q->head) {
        q->items[temp] = n;
        q->size += 1;
        q->tail = (q->tail + 1) % q->capacity;
        return true;
    }
    return false;
}

bool dequeue(PriorityQueue *q, Node **n) {
    if (pq_empty(q)) {
        return false;
    }
    *n = q->items[q->head];
    q->size -= 1;
    q->head = (q->head + 1) % q->capacity;
    return true;
}

void pq_print(PriorityQueue *q) {
    printf("[");
    for (uint32_t i = 0; i < q->size; i += 1) {
        printf("%lu", q->items[q->head + i]->frequency);
        if (i + 1 != q->size) {
            printf(", ");
        }
    }
    printf("]\n");
}
