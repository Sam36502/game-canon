#ifndef BUTTON_H
#define BUTTON_H

//
//	Button.h
//	Simple UI button library
//
//	Allows you to define buttons by their position and bounding box
//	Also lets you define their relative positions for arrow navigation
//

#include <raylib.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


#define BTN_MAX_COUNT UINT16_MAX
#define BTN_ID_NULL 0x0000


//	
//	Type Definitions
//	

typedef uint16_t Button_ID;

typedef enum {
	BTN_UP = 0x00,
	BTN_RIGHT = 0x01,
	BTN_DOWN = 0x02,
	BTN_LEFT = 0x03,
} Button_Direction;

typedef enum {
	BTN_ENABLED,	// Selectable
	BTN_DISABLED,	// Not selectable
	BTN_HOVERED,	// Cursor hovering
	BTN_ACTIVE,		// Button clicked
} Button_State;

typedef struct {
	Vector2 pos;
	Vector2 size;
	Button_State state;
	Button_ID relative[4];
} Button;


//	
//	Global Variable Declarations
//	


//	
//	Function Declarations
//	

//	Initialise Button System
//	
void BTN_init();

//	Terminate Button System
//	
void BTN_terminate();

//	Handle Drawing all buttons
//	
//	Note: Buttons are drawn in the order 
void BTN_draw_all(int x, int y, int w, int h);

//	Handle input all buttons
//	
void BTN_handle_input(int x, int y, int w, int h);

//	Create a new button
//	
Button_ID BTN_create(int x, int y, int w, int h);

#endif
