#include "../include/queue.h"

void init_queue(Queue *q) {
  pthread_cond_init(&q->cond_not_empty, NULL);
  pthread_cond_init(&q->cond_not_full, NULL);
  pthread_mutex_init(&q->mutex, NULL);

  q->size = 0;
  q->head = 0;
  q->tail = 0;
}

void enqueue(Queue *q, Frame *val) {
  pthread_mutex_lock(&q->mutex);

  while (q->size >= MAX_SIZE) {
    pthread_cond_wait(&q->cond_not_full, &q->mutex);
  }

  q->elements[q->tail] = val;
  q->tail = (q->tail + 1) % MAX_SIZE;
  q->size++;

  pthread_cond_signal(&q->cond_not_empty);
  pthread_mutex_unlock(&q->mutex);
}

Frame *dequeue(Queue *q) {
  pthread_mutex_lock(&q->mutex);

  while (q->size <= 0) {
    pthread_cond_wait(&q->cond_not_empty, &q->mutex);
  }

  Frame *head = q->elements[q->head];
  q->head = (q->head + 1) % MAX_SIZE;
  q->size--;

  pthread_cond_signal(&q->cond_not_full);
  pthread_mutex_unlock(&q->mutex);
  return head;
}
