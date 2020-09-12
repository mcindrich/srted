#include <stdlib.h>
#include <string.h>
#include <sub.h>

void sub_init(sub_t *sub) {
	for (int i = 0; i < MAX_LINES; i++) {
		utf8_string_init(&sub->text[i]);
	}
	time_init(&sub->s_time);
	time_init(&sub->e_time);
}
void sub_set_line(sub_t *sub, int ln, char *text) {
	utf8_string_from_bytes(&sub->text[ln], text, strlen(text));
}
void sub_free(sub_t *sub) {
	for (int i = 0; i < MAX_LINES; i++) {
		utf8_string_free(&sub->text[i]);
	}
}