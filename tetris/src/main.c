//	
//		TETRIS
//	
//	Reimplemented using Raylib
//	
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/beep.h"
#include "../include/track.h"
#include "../include/tetris.h"

#define FRAMERATE 60
#define TICK_DELAY 1.000f

// Sounds
#define SND_WAVE WAV_SQUARE

// Inputs
#define KEY_LEFT 263
#define KEY_RIGHT 262
#define KEY_UP 265
#define KEY_DOWN 264
#define KEY_LEFT_ALT 65
#define KEY_RIGHT_ALT 68
#define KEY_UP_ALT 87
#define KEY_DOWN_ALT 83

#define KEY_FULLSCREEN 70
#define KEY_THEME_1 49
#define KEY_THEME_2 50
#define KEY_THEME_3 51
#define KEY_THEME_4 52
#define KEY_THEME_5 53

// Game Constants
#define PLAYER_SPEED 8

// Type Definitons

// Function Declarations
void print_help();


// Globals
static float g_tick_timer = 0.0f;	// How long to wait between game ticks
static int g_colour_theme = 0;

const Color g_colour_themes[][6] = {
//		Background	Border		Tetromino Colours
	{	BLACK,		BLUE,		RED,		GREEN, BLUE,	PINK	},
	{	BLACK,		DARKGRAY,	GRAY,		LIGHTGRAY, GRAY,	LIGHTGRAY	},
	{	BLACK,		BLUE,		RED,		GREEN, BLUE,	PINK	},
	{	BLACK,		BLUE,		RED,		GREEN, BLUE,	PINK	},
	{	BLACK,		BLUE,		RED,		GREEN, BLUE,	PINK	},
	{	BLACK,		BLUE,		RED,		GREEN, BLUE,	PINK	},
};
#define NUM_CLRS 4
#define CLR_BACKGROUND (g_colour_themes[g_colour_theme][0])
#define CLR_BORDER (g_colour_themes[g_colour_theme][1])
#define CLR_TETRO(n) (g_colour_themes[g_colour_theme][n+1])


int main(int argc, char *argv[]) {

	// Parse Arguments
	if (argc > 1) {

		// Check for flags
		if (argv[1][0] == '-') {
			int len = strlen(argv[1]);
			if ((len > 1 && argv[1][1] == 'h')
				|| strcmp("--help", argv[1]) == 0
			) {
				print_help();
				return EXIT_SUCCESS;
			}
		}
	}

	// Initialise
	SetTraceLogLevel(LOG_WARNING);
	SetRandomSeed(time(NULL));
	InitWindow(1000, 800,
		"Game Canon - Tetris"
	);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(FRAMERATE);

	SND_init();
	SND_loadpreset(SND_WAVE);

	// Set up game objects
	clear_board();
	Tetromino tetro = {
		.bitmap = g_tetromino_bitmaps[GetRandomValue(0, 6)],
		.pos_x = 1,
		.pos_y = 0,
		.rotation = 0,
		.colour = 1,
	};

	// Main Loop
	static int frame_counter = 0;
	while (!WindowShouldClose()) {
		TRK_update();

		// DEBUG: Print key presses
		int k = GetKeyPressed();
		if (k != 0) printf("---> Key Pressed: %i\n", k);
		fflush(stdout);

		if (IsKeyPressed(KEY_FULLSCREEN)) ToggleFullscreen();
		if (IsKeyPressed(KEY_THEME_1)) g_colour_theme = 0;
		if (IsKeyPressed(KEY_THEME_2)) g_colour_theme = 1;
		if (IsKeyPressed(KEY_THEME_3)) g_colour_theme = 2;
		if (IsKeyPressed(KEY_THEME_4)) g_colour_theme = 3;
		if (IsKeyPressed(KEY_THEME_5)) g_colour_theme = 4;

		static int test_tet = 0;
		static int test_rot = 0;
		if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_LEFT_ALT)) && tetro.pos_x > 0) tetro.pos_x--;
		if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT_ALT)) && tetro.pos_x < BOARD_WIDTH - 4) tetro.pos_x++;
		if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_DOWN_ALT)) {
			test_tet++;
			test_tet %= 6;

			tetro.bitmap = g_tetromino_bitmaps[test_tet];
			tetro.pos_x = 1;
			tetro.pos_y = 0;
			tetro.rotation = 0;
			tetro.colour = GetRandomValue(1, 4);
		}

		// Update Objects
		if (g_tick_timer <= 0.0f) {
			tetro.pos_y++;
			g_tick_timer = TICK_DELAY;
		} else {
			g_tick_timer -= GetFrameTime();
			if (g_tick_timer < 0.0f) g_tick_timer = 0.0f;
		}

		// Draw Elements
		BeginDrawing();
		ClearBackground(CLR_BORDER);

		// Clear playfield/board background
		draw_board(CLR_BACKGROUND, (Color *) &(g_colour_themes[g_colour_theme][1]), 4);
		draw_tetromino(tetro, (Color *) &(g_colour_themes[g_colour_theme][1]), 4);

		// Draw Tetramino
		EndDrawing();
		frame_counter++;
	}

	// Terminate
	SND_terminate();
	CloseWindow();

	return EXIT_SUCCESS;
}

void print_help() {
	puts("tetris - A simple tetris implementation");
	puts("");
	puts("Options:");
	puts("  -h,--help		Print this information and exit");
	puts("");
}
