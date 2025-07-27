#ifndef TRACK_H
#define TRACK_H
//
//	Track.h
//	Basic macro system
//
#include <raylib.h>

#include "../include/beep.h"


//	
//	Constants
//	

#define TRK_QUEUE_LEN 64


//	
//	Type Definitions
//	

typedef struct {
	float freq;
	float dur;
} TRK_Note;


//	
//	Function Declarations
//	

//	Called every frame to progress track counters
//
void TRK_update();


//	Adds a note to the list of notes to play
//
//	Will simply return without doing anything if the queue is already full!
void TRK_queue_note(float freq, float dur);

//	Starts playing the notes in the queue
//
void TRK_queue_play();

//	Stops playing the notes in the queue
//
void TRK_queue_stop();

//	Clears the queue
//
void TRK_queue_clear();

#endif
