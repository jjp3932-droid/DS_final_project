#pragma once
#ifndef QUEUE_H
#define QUEUE_H

#define QSIZE 20

typedef struct {
    int data[QSIZE];   // 역 ID를 담는 칸
    int front;         // 다음에 꺼낼 칸
    int rear;          // 다음에 넣을 칸
    int count;         // 들어 있는 개수
} Queue;

void initQueue(Queue* q);
void enqueue(Queue* q, int v);
int  dequeue(Queue* q);
int  isEmpty(Queue* q);

#endif