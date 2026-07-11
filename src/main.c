#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/queue.h"
#include "../include/video_handler.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <sys/_pthread/_pthread_t.h>

void *run_producer(void *producer_args) {
  int status = produce_frames(producer_args);

  if (status > 0) {
    exit(status);
  }

  return NULL;
}

void *run_consummer(void *consumer_args) {
  consume_frames(consumer_args);
  return NULL;
}

int main(int argc, char **argv) {

  AVFormatContext *format_ctx = NULL;
  int v_stream = -1;
  int *video_data = malloc(sizeof(int) * 3);

  int init_status = init_format_context(&format_ctx, &v_stream, argc, argv);
  if (init_status > 0) {
    return init_status;
  }

  int status = extract_data(format_ctx, v_stream, video_data);
  if (status > 0) {
    return status;
  }

  printf("Video resolution is %dx%d\nFrame rate is %d FPS\n", video_data[0],
         video_data[1], video_data[2]);

  Queue *q = malloc(sizeof(Queue));
  init_queue(q);

  ProducerArgs *producer_args = malloc(sizeof(ProducerArgs));
  producer_args->format_ctx = format_ctx;
  producer_args->q = q;
  producer_args->stream_id = v_stream;
  producer_args->w = video_data[0];
  producer_args->h = video_data[1];

  ConsumerArgs *consumer_args = malloc(sizeof(ConsumerArgs));
  consumer_args->q = q;
  consumer_args->w = video_data[0];
  consumer_args->h = video_data[1];
  consumer_args->treshold = 5;

  pthread_t producer, consummer;

  pthread_create(&producer, NULL, run_producer, (void *)producer_args);
  pthread_create(&consummer, NULL, run_consummer, (void *)consumer_args);

  pthread_join(producer, NULL);
  pthread_join(consummer, NULL);

  free(video_data);
  free(q);
  free(producer_args);
  free(consumer_args);

  avformat_close_input(&format_ctx);
  return 0;
}
