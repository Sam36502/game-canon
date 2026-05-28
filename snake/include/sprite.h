#ifndef SPRITE_H
#define SPRITE_H
//	
//	Functions for handling moving 2D objects
//	

#include <raylib.h>
#include <stdlib.h>
#include <stdint.h>

//	
//	Constant Definitions
//	

#define PIXEL_SIZE (int)(GetRenderHeight() / 100)

//	
//	Type Definitions
//	

typedef struct {
	Rectangle rect; // pos & size
	Vector2 vel;
	Vector2 spritesheet_origin;
	Color clr;
	int frame_count;
	int frame_index;
	uint8_t *bitmap;
} SPR_Sprite;


//	
//	Function Declarations
//	

//	Loads a spritesheet from an image
//
//	Loaded spritesheet is stored in a private global variable
void SPR_load_spritesheet(Image img);

//	Unloads the previously loaded spritesheet
//
void SPR_unload_spritesheet();

//	Draws a sprite to the screen
//	
void SPR_draw(SPR_Sprite sprite);

#endif
