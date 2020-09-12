#include <stdlib.h>
#include <sub_list.h>

void sub_list_init(sub_list_t *list) {
	list->count = 0;
	list->subs = NULL;
}
int sub_list_alloc(sub_list_t *list, int count) {
	list->subs = (sub_t *) malloc(sizeof(sub_t) * count);
	if (!list->subs) {
		return 1;
	}
	list->count = count;
	return 0;
}

sub_t *sub_list_get_sub(sub_list_t *list, int pos) {
	sub_t *ptr = NULL;
	if (pos < list->count)
		ptr = &list->subs[pos];
	return ptr;
}

void sub_list_free(sub_list_t *list) {
	if (list->subs) {
		for (int i = 0; i < list->count; i++) {
			sub_free(&list->subs[i]);
		}
		free(list->subs);
		list->subs = NULL;
		list->count = 0;
	}
}