#include "../include/frame_handler.h"
#include "libavutil/rational.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void to_grayscale(uint8_t *rgb, uint8_t *gray, int width, int height) {
  for (int i = 0; i < width * height; ++i) {
    uint8_t r = rgb[i * 3 + 0];
    uint8_t g = rgb[i * 3 + 1];
    uint8_t b = rgb[i * 3 + 2];

    gray[i] = 0.299 * r + 0.587 * g + 0.114 * b;
  }
}

bool has_motion(Frame *frame1, Frame *frame2, int threshold) {
  int diff_sum = 0;

  for (int i = 0; i < frame1->w * frame1->h; ++i) {
    diff_sum += abs((int)frame1->frame[i] - (int)frame2->frame[i]);
  }

  int avg = diff_sum / (frame1->w * frame1->h);

  return avg >= threshold;
}

Frame *init_frame(uint8_t *frame, AVRational time_base, int64_t pts, int w,
                  int h) {
  Frame *new_frame = malloc(sizeof(Frame));
  new_frame->frame = malloc(w * h * sizeof(uint8_t));
  memcpy(new_frame->frame, frame, w * h);
  new_frame->w = w;
  new_frame->h = h;
  new_frame->pts = pts;
  new_frame->time_base = time_base;

  return new_frame;
}
