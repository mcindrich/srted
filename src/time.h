#pragma once

#define S_MSEC 1000
#define M_MSEC (60 * S_MSEC)
#define H_MSEC (60 * M_MSEC)

typedef struct time srt_time_t;

struct time {
  int h;
  int m;
  int s;
  int ms;
};

void time_init(srt_time_t *time);
void time_add_msec(srt_time_t *time, long long ms);