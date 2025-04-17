#include "queue.h"
#include <stdlib.h>

void initQueue(Queue* q) {
    q->front = q->rear = NULL;
    q->size = 0;
}

void push(Queue* q, BTreeNode* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    if (q->rear) {
        q->rear->next = newNode;
    } else {
        q->front = newNode;
    }
    q->rear = newNode;
    q->size++;
}

void pop(Queue* q) {
    if (!q->front) return;
    Node* temp = q->front;
    q->front = q->front->next;
    if (!q->front) q->rear = NULL;
    free(temp);
    q->size--;
}

BTreeNode* front(Queue* q) {
    return q->front ? q->front->data : NULL;
}

bool isEmpty(Queue* q) {
    return q->front == NULL;
}

void clear(Queue* q) {
    while (!isEmpty(q)) {
        pop(q);
    }
}

int getSize(Queue* q) {
    return q->size;
}