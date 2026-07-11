#include "../include/video_handler.h"
#include "../include/frame_handler.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/codec.h"
#include "libavcodec/codec_par.h"
#include "libavcodec/packet.h"
#include "libavutil/error.h"
#include "libavutil/frame.h"
#include "libavutil/rational.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

int init_format_context(AVFormatContext **format_ctx, int *v_stream, int argc,
                        char **argv) {
  if (argc < 2) {
    printf("Usage %s <input_file>\n", argv[0]);
    return 1;
  }

  char *input_file = argv[1];

  if (avformat_open_input(format_ctx, input_file, NULL, NULL) < 0) {
    printf("Could not open input file: %s\n", input_file);
    return 2;
  }

  if (avformat_find_stream_info(*format_ctx, NULL) < 0) {
    printf("Could not find stream informatio\nn");
    avformat_close_input(format_ctx);
    return 3;
  }

  for (int i = 0; i < (*format_ctx)->nb_streams; ++i) {
    if ((*format_ctx)->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      *v_stream = i;
      break;
    }
  }

  if (*v_stream == -1) {
    printf("Could not find video stream\n");
    avformat_close_input(format_ctx);
    return 4;
  }

  return 0;
}

int produce_frames(ProducerArgs *args) {
  AVCodecParameters *codec_pars =
      args->format_ctx->streams[args->stream_id]->codecpar;
  const AVCodec *codec = avcodec_find_decoder(codec_pars->codec_id);

  if (!codec) {
    printf("Codec not found!\n");
    return 5;
  }

  AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);

  if (!codec_ctx) {
    printf("Could not allocate codec context!\n");
    return 6;
  }

  avcodec_parameters_to_context(codec_ctx, codec_pars);

  if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
    printf("Could not open codec\n");
    avcodec_free_context(&codec_ctx);
    return 7;
  }

  AVFrame *frame = av_frame_alloc();
  AVPacket *packet = av_packet_alloc();

  uint8_t *current_frame = malloc(args->w * args->h * sizeof(uint8_t));

  AVRational time_base = args->format_ctx->streams[args->stream_id]->time_base;

  while (av_read_frame(args->format_ctx, packet) >= 0) {
    if (packet->stream_index == args->stream_id) {
      int ret = avcodec_send_packet(codec_ctx, packet);
      if (ret < 0) {
        printf("Error sending packet for decoding\n");
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codec_ctx);
        return 8;
      }

      while (ret >= 0) {
        ret = avcodec_receive_frame(codec_ctx, frame);

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
          break;
        } else if (ret < 0) {
          printf("Error during decoding\n");
          break;
        }
        for (int row = 0; row < args->h; ++row) {
          memcpy(current_frame + row * args->w,
                 frame->data[0] + row * frame->linesize[0], args->w);
        }

        Frame *new_frame =
            init_frame(current_frame, time_base, frame->pts, args->w, args->h);

        enqueue(args->q, new_frame);
      }
    }

    av_packet_unref(packet);
  }

  enqueue(args->q, NULL);

  av_packet_free(&packet);
  av_frame_free(&frame);
  avcodec_free_context(&codec_ctx);

  free(current_frame);
  return 0;
}

void consume_frames(ConsumerArgs *args) {
  Frame *f_prev = NULL;
  Frame *f = dequeue(args->q);
  while (f != NULL) {
    bool has_motion_v = false;
    if (f_prev != NULL) {
      has_motion_v = has_motion(f_prev, f, args->treshold);
      free(f_prev->frame);
      free(f_prev);
    }

    if (has_motion_v) {
      double seconds = f->pts * av_q2d(f->time_base);
      int h = (int)(seconds / 3600) % 24;
      int m = (int)(seconds / 60) % 60;
      int s = (int)seconds % 60;
      printf("[%d:%d:%d] MOTION DETECTED!!!\n", h, m, s);
    }

    f_prev = f;
    f = dequeue(args->q);
  }
  if (f_prev != NULL) {
    free(f_prev->frame);
    free(f_prev);
  }
}

int extract_data(AVFormatContext *format_ctx, int v_stream, int *target) {
  AVCodecParameters *cod_par = format_ctx->streams[v_stream]->codecpar;

  int width = cod_par->width;
  int height = cod_par->height;
  AVRational fps = format_ctx->streams[v_stream]->avg_frame_rate;

  target[0] = width;
  target[1] = height;
  target[2] = (int)av_q2d(fps);
  return 0;
}
