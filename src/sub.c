#include <stdlib.h>
#include <string.h>
#include <sub.h>

void sub_init(sub_t *sub) {
	for (int i = 0; i < MAX_LINES; i++) {
		sub->text[i] = 0;
	}
	time_init(&sub->s_time);
	time_init(&sub->e_time);
}
void sub_set_line(sub_t *sub, int ln, char *text) {
	int len = strlen(text);
	sub->text[ln] = malloc(sizeof(char) * len);
	memcpy(sub->text[ln], text, len);
	sub->text[ln][len - 1] = 0;
}
void sub_free(sub_t *sub) {
	for (int i = 0; i < MAX_LINES; i++) {
		if (sub->text[i]) {
			free(sub->text[i]);
		}
	}
}