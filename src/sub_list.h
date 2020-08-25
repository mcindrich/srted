#pragma once

#include "sub.h"

typedef struct sub_list sub_list_t;

struct sub_list {
  sub_t *subs;
  int count;
};

void sub_list_init(sub_list_t *list);
int sub_list_alloc(sub_list_t *list, int count);
sub_t *sub_list_get_sub(sub_list_t *list, int pos);
void sub_list_free(sub_list_t *list);