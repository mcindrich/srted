#include <time.h>

void time_init(srt_time_t *time) {
	time->h = time->m = time->s = time->ms = 0;
}
void time_add_msec(srt_time_t *time, long long ms) {
	// calc current msec
	long long msec =
			time->h * H_MSEC + time->m * M_MSEC + time->s * S_MSEC + time->ms;
	// add given ms
	msec += ms;
	// get new time
	time->h = msec / H_MSEC;
	msec -= time->h * H_MSEC;
	time->m = msec / M_MSEC;
	msec -= time->m * M_MSEC;
	time->s = msec / S_MSEC;
	msec -= time->s * S_MSEC;
	time->ms = msec;
}