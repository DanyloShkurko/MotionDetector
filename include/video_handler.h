#pragma once

#include "../include/queue.h"
#include "libavformat/avformat.h"
#include <stdint.h>

typedef struct {
  AVFormatContext *format_ctx;
  Queue *q;
  int stream_id;
  int w;
  int h;
} ProducerArgs;

typedef struct {
  Queue *q;
  int w;
  int h;
  int treshold;
} ConsumerArgs;

int init_format_context(AVFormatContext **format_ctx, int *v_stream, int argc,
                        char **argv);
int produce_frames(ProducerArgs *args);
void consume_frames(ConsumerArgs *args);

int extract_data(AVFormatContext *format_ctx, int v_stream, int *target);
