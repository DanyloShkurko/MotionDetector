#pragma once
#include "frame_handler.h"
#define MAX_SIZE 4

#include <pthread.h>

typedef struct Queue {

  pthread_cond_t cond_not_empty;
  pthread_cond_t cond_not_full;
  pthread_mutex_t mutex;

  Frame *elements[MAX_SIZE];

  int size;
  int head;
  int tail;

} Queue;

void init_queue(Queue *q);
void enqueue(Queue *q, Frame *val);
Frame *dequeue(Queue *q);
