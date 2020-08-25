#pragma once

#include <common.h>
#include <time.h>

typedef struct sub sub_t;

struct sub {
  srt_time_t s_time;
  srt_time_t e_time;
  char *text[MAX_LINES];
};

void sub_init(sub_t *sub);
void sub_set_line(sub_t *sub, int ln, char *text);
void sub_free(sub_t *sub);