#include <stdio.h>
#include "queue1.h"

void initQueue(Queue* q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
}

void enqueue(Queue* q, int v) {
    if (q->count == QSIZE) {            // 가득 참
        printf("큐가 가득 찼습니다\n");
        return;
    }
    q->data[q->rear] = v;               // rear 칸에 넣고
    q->rear = (q->rear + 1) % QSIZE;    // rear를 한 칸 뒤로 (19 다음은 0)
    q->count++;
}

int dequeue(Queue* q) {
    if (q->count == 0) {                // 비어 있음
        printf("큐가 비었습니다\n");
        return -1;
    }
    int v = q->data[q->front];          // front 칸에서 꺼내고
    q->front = (q->front + 1) % QSIZE;  // front를 한 칸 뒤로
    q->count--;
    return v;
}

int isEmpty(Queue* q) {
    return q->count == 0;               // 비었으면 1, 아니면 0
}