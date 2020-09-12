#pragma once

#include "sub_list.h"
#include <stdio.h>

typedef struct srt srt_t;

struct srt {
	sub_list_t subs;
};

void srt_init(srt_t *srt);
void srt_print(srt_t *srt, FILE *file);
int srt_load_from_file(srt_t *srt, const char *fn);
int srt_write_to_file(srt_t *srt, const char *fn);
void srt_add_hours(srt_t *srt, int h);
void srt_add_minutes(srt_t *srt, int m);
void srt_add_seconds(srt_t *srt, int s);
void srt_add_miliseconds(srt_t *srt, long long ms);
void srt_free(srt_t *srt);