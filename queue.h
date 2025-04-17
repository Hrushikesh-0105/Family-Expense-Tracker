#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "B_Tree.h"

typedef struct Node {
    BTreeNode* data;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
    int size;
} Queue;

void initQueue(Queue* q);
void push(Queue* q, BTreeNode* data);
void pop(Queue* q);
BTreeNode* front(Queue* q);
bool isEmpty(Queue* q);
void clear(Queue* q);
int getSize(Queue* q);

#endif