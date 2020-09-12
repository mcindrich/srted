#include <common.h>
#include <srt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void srt_init(srt_t *srt) {
	sub_list_init(&srt->subs);
}
void _parse_timecode(char *buffer, srt_time_t *start, srt_time_t *end) {
	const char *split = "--> :,";
	int parts[8];
	char *token = strtok(buffer, split);

	int part_cnt = 0;

	while (token != NULL) {
		// printf("TOK: %s\n", token);
		parts[part_cnt] = atoi(token);
		token = strtok(NULL, split);
		++part_cnt;
	}
	memcpy(start, parts, sizeof(int) * 4);
	memcpy(end, parts + 4, sizeof(int) * 4);

	// printf("%d:%d:%d\n", start->h, start->m, start->s);
	// printf("%d:%d:%d\n", end->h, end->m, end->s);
}

int srt_load_from_file(srt_t *srt, const char *fn) {
	FILE *file = fopen(fn, "r");
	char buffer[BUFFER_SIZE];

	enum PARSING_STATE { PARSING_COUNTER = 0,
											 PARSING_TIMECODE,
											 PARSING_TEXT };

	if (!file) {
		ERR("unable to open file for reading");
		return -1;
	}
	int count = 0;
	// start with counter parsing
	int p_state = PARSING_COUNTER;
	// first precount subtitles
	while (fgets(buffer, BUFFER_SIZE, file)) {
		switch (p_state) {
			case PARSING_COUNTER: {
				++count;
				p_state = PARSING_TIMECODE;
				break;
			}
			case PARSING_TIMECODE: {
				// printf("TIME: %s\n", buffer);
				p_state = PARSING_TEXT;
				break;
			}
			case PARSING_TEXT: {
				if ((buffer[0] == '\r' && buffer[1] == '\n') || buffer[0] == '\n') {
					// blank line
					p_state = PARSING_COUNTER;
				} else {
					// printf("TEXT: '%s'\n", buffer);
				}
				break;
			}
			default: {
				// error
				break;
			}
		}
	}
	if (sub_list_alloc(&srt->subs, count)) {
		ERR("unable to allocate space for a list of subtitles");
		return 1;
	}
	// parse again and create a list of subs
	count = p_state = PARSING_COUNTER;
	// line number for text parsing
	int line_num = 0;
	// return fptr to 0
	fseek(file, 0, SEEK_SET);
	sub_t *curr_sub = NULL;
	while (fgets(buffer, BUFFER_SIZE, file)) {
		switch (p_state) {
			case PARSING_COUNTER: {
				curr_sub = sub_list_get_sub(&srt->subs, count);
				++count;
				p_state = PARSING_TIMECODE;
				sub_init(curr_sub);
				break;
			}
			case PARSING_TIMECODE: {
				p_state = PARSING_TEXT;
				_parse_timecode(buffer, &curr_sub->s_time, &curr_sub->e_time);
				break;
			}
			case PARSING_TEXT: {
				if ((buffer[0] == '\r' && buffer[1] == '\n') || buffer[0] == '\n') {
					// blank line
					p_state = PARSING_COUNTER;
					line_num = 0;
				} else {
					sub_set_line(curr_sub, line_num, buffer);
					++line_num;
					// printf("TEXT: '%s'\n", buffer);
				}
				break;
			}
			default: {
				// error
				break;
			}
		}
	}
	fclose(file);
	return 0;
}

void srt_print(srt_t *srt, FILE *file) {
	sub_list_t list = srt->subs;
	for (int i = 0; i < list.count; i++) {
		fprintf(file, "%d\n", i + 1);
		sub_t *sub = sub_list_get_sub(&list, i);
		fprintf(file, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\n",
						sub->s_time.h, sub->s_time.m, sub->s_time.s, sub->s_time.ms,
						sub->e_time.h, sub->e_time.m, sub->e_time.s, sub->e_time.ms);
		for (int i = 0; i < MAX_LINES; i++) {
			char *line = sub->text[i];
			if (line) {
				fprintf(file, "%s\n", line);
			}
		}
		fprintf(file, "\n");
	}
}

int srt_write_to_file(srt_t *srt, const char *fn) {
	FILE *file = fopen(fn, "w");
	if (!file) {
		return 1;
	}
	srt_print(srt, file);
	fclose(file);
	return 0;
}

void srt_add_hours(srt_t *srt, int h) {
	srt_add_miliseconds(srt, (long long) h * H_MSEC);
}
void srt_add_minutes(srt_t *srt, int m) {
	srt_add_miliseconds(srt, (long long) m * M_MSEC);
}
void srt_add_seconds(srt_t *srt, int s) {
	srt_add_miliseconds(srt, (long long) s * S_MSEC);
}

void srt_add_miliseconds(srt_t *srt, long long ms) {
	sub_list_t list = srt->subs;
	for (int i = 0; i < list.count; i++) {
		sub_t *sub = sub_list_get_sub(&list, i);
		time_add_msec(&sub->s_time, ms);
		time_add_msec(&sub->e_time, ms);
	}
}

void srt_free(srt_t *srt) {
	sub_list_free(&srt->subs);
}