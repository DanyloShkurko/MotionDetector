#pragma once
#include "libavutil/rational.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct Frame {
  uint8_t *frame;
  int64_t pts;
  AVRational time_base;
  int w;
  int h;
} Frame;

void to_grayscale(uint8_t *rgb, uint8_t *gray, int width, int height);
bool has_motion(Frame *frame1, Frame *frame2, int threshold);
Frame *init_frame(uint8_t *frame, AVRational time_base, int64_t pts, int w,
                  int h);
