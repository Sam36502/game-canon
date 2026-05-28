//	
//		SNAKE
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

#define FRAMERATE 60
#define TICK_DELAY 0.200f

#define BOARD_WIDTH 20 
#define BOARD_HEIGHT 16 

#define TXT_SIZE_SCORE 50
#define TXT_SIZE_MSG 100

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

// Function Declarations
void print_help();
void reset_snake(Vector2 body[], int *len, int *dir);
Vector2 generate_apple_pos(Vector2 avoid[], int len);
bool point_collision(Vector2 p, Vector2 line[], int line_len);


// Globals
static float g_tick_timer = 0.0f;	// How long to wait between game ticks
static int g_colour_theme = 0;

const Color g_colour_themes[5][4] = {
//		Background	Apple		Snake		Score
	{	BLACK,		RED,		GREEN,		WHITE	},
	{	BLACK,		WHITE,		WHITE,		WHITE	},
	{	BLACK,		ORANGE,		ORANGE,		ORANGE	},
	{	DARKBLUE,	BLUE,		BLUE,		BLUE	},
	{	WHITE,		BLACK,		BLACK,		BLACK	},
};
#define CLR_BACKGROUND (g_colour_themes[g_colour_theme][0])
#define CLR_APPLE (g_colour_themes[g_colour_theme][1])
#define CLR_SNAKE (g_colour_themes[g_colour_theme][2])
#define CLR_DIGIT (g_colour_themes[g_colour_theme][3])


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
		"Game Canon - Snake"
	);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(FRAMERATE);

	SND_init();
	SND_loadpreset(SND_WAVE);
	char *message = NULL;

	// Set up game objects
	Vector2 snake_body[BOARD_WIDTH * BOARD_HEIGHT];
	Vector2 *snake_head = &(snake_body[0]);
	int snake_len;
	int snake_dir;
	int new_dir = -1;
	reset_snake(snake_body, &snake_len, &snake_dir);

	Vector2 apple_pos = generate_apple_pos(snake_body, snake_len);

	// Main Loop
	static int frame_counter = 0;
	while (!WindowShouldClose()) {
		TRK_update();

		// DEBUG: Print key presses
		//int k = GetKeyPressed();
		//if (k != 0) printf("---> Key Pressed: %i\n", k);
		//fflush(stdout);

		if (IsKeyPressed(KEY_FULLSCREEN)) ToggleFullscreen();
		if (IsKeyPressed(KEY_THEME_1)) g_colour_theme = 0;
		if (IsKeyPressed(KEY_THEME_2)) g_colour_theme = 1;
		if (IsKeyPressed(KEY_THEME_3)) g_colour_theme = 2;
		if (IsKeyPressed(KEY_THEME_4)) g_colour_theme = 3;
		if (IsKeyPressed(KEY_THEME_5)) g_colour_theme = 4;

		if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_LEFT_ALT)) new_dir = 0;
		if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_UP_ALT)) new_dir = 1;
		if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT_ALT)) new_dir = 2;
		if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_DOWN_ALT)) new_dir = 3;

		// Update Objects
		if (g_tick_timer <= 0.0f) {

			if (Vector2Equals(apple_pos, *snake_head)) {
				SND_beep(880.0f, 0.100f);
				apple_pos = generate_apple_pos(snake_body, snake_len);
				snake_len++;
			}

			for (int i=snake_len-1; i>0; i--) snake_body[i] = snake_body[i-1];

			if (new_dir > -1 && (new_dir & 0b1) != (snake_dir & 0b1)) {
				snake_dir = new_dir;
				new_dir = -1;
			}
			if (snake_dir & 0b1) snake_head->y += snake_dir - 2;
			else snake_head->x += snake_dir - 1;

			// Check for collisions
			if (snake_head->x < 0
				|| snake_head->x >= BOARD_WIDTH
				|| snake_head->y < 0
				|| snake_head->y >= BOARD_HEIGHT
				|| point_collision(*snake_head, snake_body+1, snake_len-1)
			) {
				g_tick_timer = TICK_DELAY * 10;
				message = "You died lmao";
				
				SND_beep(220.0f, 0.500f);
				reset_snake(snake_body, &snake_len, &snake_dir);
				continue;
			}

			if (message != NULL) message = NULL;
			g_tick_timer = TICK_DELAY;
		} else {
			g_tick_timer -= GetFrameTime();
			if (g_tick_timer < 0.0f) g_tick_timer = 0.0f;
		}

		// Draw Elements
		BeginDrawing();
		ClearBackground(BLUE);

		const int px_sz = GetRenderWidth() / BOARD_WIDTH;
		const int offset_x = GetRenderWidth() / 2 - (BOARD_WIDTH * px_sz) / 2;
		const int offset_y = GetRenderHeight() / 2 - (BOARD_HEIGHT * px_sz) / 2;

		// Clear playfield
		DrawRectangle(
			offset_x, offset_y,
			px_sz * BOARD_WIDTH, px_sz * BOARD_HEIGHT,
			CLR_BACKGROUND
		);

		// Draw Apple
		DrawRectangle(
			offset_x + px_sz * apple_pos.x, offset_y + px_sz * apple_pos.y,
			px_sz, px_sz, CLR_APPLE
		);

		// Snake body
		for (int i=0; i<snake_len; i++) {
			Vector2 seg = snake_body[i];
			DrawRectangle(
				offset_x + px_sz * seg.x, offset_y + px_sz * seg.y,
				px_sz, px_sz, CLR_SNAKE
			);
		}

		// Draw Score
		DrawText(TextFormat("SCORE: %i00", snake_len),
			offset_x + 10, offset_y + 10, TXT_SIZE_SCORE, CLR_DIGIT
		);

		// Draw a message in the middle of the screen
		if (message != NULL) {
			DrawText(message,
				offset_x + (BOARD_WIDTH * px_sz) / 2 - MeasureText(message, TXT_SIZE_MSG) / 2,
				offset_y + (BOARD_WIDTH * px_sz) / 2 - TXT_SIZE_MSG / 2,
				TXT_SIZE_MSG, CLR_DIGIT
			);
		}

		EndDrawing();
		frame_counter++;
	}

	// Terminate
	SND_terminate();
	CloseWindow();

	return EXIT_SUCCESS;
}

void print_help() {
	puts("snake - A simple snake game");
	puts("");
	puts("Options:");
	puts("  -h,--help		Print this information and exit");
	puts("");
	puts("Controls:");
	puts("  W,A,S,D		Control which direction the snake is moving");
	puts("  1,2,3,4,5	Set the colour scheme to use");
	puts("");
}

Vector2 generate_apple_pos(Vector2 avoid[], int len) {
	Vector2 new = (Vector2){
		GetRandomValue(0, BOARD_WIDTH-1),
		GetRandomValue(0, BOARD_HEIGHT-1),
	};
	while (point_collision(new, avoid, len)) {
		new = (Vector2){
			GetRandomValue(0, BOARD_WIDTH-1),
			GetRandomValue(0, BOARD_HEIGHT-1),
		};
	}

	return new;
}

void reset_snake(Vector2 body[], int *len, int *dir) {
	body[0] = (Vector2){ 4, 4 };
	body[1] = (Vector2){ 3, 4 };
	body[2] = (Vector2){ 2, 4 };
	*len = 3;
	*dir = 2; // 0=left, 1=up, 2=right, 3=down
}

bool point_collision(Vector2 p, Vector2 line[], int line_len) {
	for (int i=0; i<line_len; i++) {
		if (Vector2Equals(p, line[i])) return true;
	}

	return false;
}
