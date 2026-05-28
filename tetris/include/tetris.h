#ifndef TETRIS_H
#define TETRIS_H

#include <raylib.h>
#include <stdint.h>

//	
//	Macro Constants
//	
#define BOARD_WIDTH 10 
#define BOARD_HEIGHT 16 


//	
//	Type Definitions
//	
typedef struct {
	uint8_t colour;
	uint8_t pos_x;
	uint8_t pos_y;
	uint8_t rotation;
	uint16_t bitmap;
} Tetromino;

//	
//	Global Variables
//	
extern const uint16_t g_tetromino_bitmaps[7];
extern uint8_t g_board[BOARD_HEIGHT][BOARD_WIDTH];


//	
//	Function Declarations
//	

void clear_board();

void draw_tetromino(Tetromino tet, Color palette[], int num_clrs);

void draw_board(Color bg_clr, Color palette[], int num_clrs);

void board_check_bottom();


#endif
