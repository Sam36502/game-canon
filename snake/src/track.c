#include "../include/track.h"
#include <stdio.h>

static TRK_Note __note_queue[TRK_QUEUE_LEN];
static int __qp_read = 0;
static int __qp_write = 0;

static float __current_timer = 0.0f;
static bool __is_playing = false;


// Fetch next note in the queue
// 
// Returns the number of notes read (0 if empty or NULL pointer passed)
int __queue_pop(TRK_Note *note) {
	if (note == NULL) return 0;
	if (__qp_read == __qp_write) return 0;

	*note = __note_queue[__qp_read++];
	__qp_read %= TRK_QUEUE_LEN;
	return 1;
}

//	Print out a graphical representation of the whole queue for debugging
void __queue_print() {
	printf("Queue State:\n");
	printf("  R: %2.2i %*s%s\n      [", __qp_read, __qp_read * 3, "", "vv ");
	for (int i=0; i<TRK_QUEUE_LEN; i++) {
		printf("% 2.2i", (int) __note_queue[i].freq);
	}
	printf(" ]\n  W: %2.2i %*s%s\n", __qp_write, __qp_write * 3, "", "^^ ");
	fflush(stdout);
}


void TRK_update() {
	if (!__is_playing) return;

	__current_timer -= GetFrameTime();
	if (__current_timer > 0.0f) return;

	TRK_Note note;
	int n = __queue_pop(&note);
	if (n < 1) {
		// End of the queue
		__current_timer = 0.0f;
		return;
	}

	SND_beep(note.freq, note.dur);
	__current_timer = note.dur;
}

void TRK_queue_note(float freq, float dur) {
	// Check if the queue is full
	if ((__qp_write + 1) % TRK_QUEUE_LEN == __qp_read) return;
	
	__note_queue[__qp_write++] = (TRK_Note){ freq, dur };
	__qp_write %= TRK_QUEUE_LEN;
}

void TRK_queue_play() {
	__is_playing = true;
}

void TRK_queue_stop() {
	__is_playing = false;
}

void TRK_queue_clear() {
	__qp_write = 0;
	__qp_read = 0;
	__is_playing = false;
	__current_timer = 0.0f;
}

