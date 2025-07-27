//	
//		SPACE INVADERS
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
#include "../include/sprite.h"

#define FRAMERATE 60
#define ANIM_SPEED 0.500f

// Sounds
#define SND_WAVE WAV_SQUARE

// Inputs
#define KEY_LEFT 263
#define KEY_RIGHT 262
#define KEY_FULLSCREEN 70
#define KEY_THEME_1 49
#define KEY_THEME_2 50
#define KEY_THEME_3 51
#define KEY_THEME_4 52
#define KEY_THEME_5 53

// Game Constants
#define PLAYER_SPEED 8

// Function Declarations
void reset_ball(Vector2 *pos, Vector2 *vel);
void draw_score(int x, int n);
void play_jingle();
void print_help();

// Globals
static float g_pause_movement = 0.0f;	// How long to pause movement for
static int g_colour_theme = 0;

const Color g_colour_themes[5][4] = {
//		Background	Ball		Paddles		Score
	{	BLACK,		WHITE,		WHITE,		WHITE	},
	{	BLACK,		ORANGE,		ORANGE,		ORANGE	},
	{	DARKBLUE,	BLUE,		BLUE,		BLUE	},
	{	BLACK,		RED,		GREEN,		WHITE	},
	{	WHITE,		BLACK,		BLACK,		BLACK	},
};
#define CLR_BACKGROUND (g_colour_themes[g_colour_theme][0])
#define CLR_BALL (g_colour_themes[g_colour_theme][1])
#define CLR_PADDLE (g_colour_themes[g_colour_theme][2])
#define CLR_DIGIT (g_colour_themes[g_colour_theme][3])

extern const uint8_t _binary_resources_spritesheet_bin_start[];
extern const uint8_t _binary_resources_spritesheet_bin_end[];


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
	//SetTraceLogLevel(LOG_WARNING);
	SetRandomSeed(time(NULL));
	InitWindow(1000, 800,
		"Game Canon - Space Invaders"
	);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(FRAMERATE);

	SND_init();
	SND_loadpreset(SND_WAVE);

	// Set up game objects
	SPR_Sprite game_sprites[] = {
		{
			.rect = {
				GetRenderWidth() / 2.0, GetRenderHeight() - 80.0,
				13, 8,
			},
			.vel = { 0.0, 0.0 },
			.clr = CLR_PADDLE,
			.frame_count = 1,
			.frame_index = 0,
			.spritesheet_origin = { 0, 0 },
		},{
			.rect = {
				10, 10,
				11, 8,
			},
			.vel = { 4.0, 0.0 },
			.clr = GREEN,
			.frame_count = 2,
			.frame_index = 0,
			.spritesheet_origin = { 0, 8 },
		},{
			.rect = {
				10, 10 + 10 * PIXEL_SIZE,
				12, 8,
			},
			.vel = { 4.0, 0.0 },
			.clr = GREEN,
			.frame_count = 2,
			.frame_index = 0,
			.spritesheet_origin = { 0, 16 },
		},{
			.rect = {
				10, 10 + 20 * PIXEL_SIZE,
				8, 8,
			},
			.vel = { 4.0, 0.0 },
			.clr = GREEN,
			.frame_count = 2,
			.frame_index = 0,
			.spritesheet_origin = { 0, 24 },
		}
	};
	#define SPRITE_COUNT (sizeof(game_sprites)/sizeof(SPR_Sprite))

	SPR_Sprite *player_ship = &(game_sprites[0]);

	Image img_spritesheet = LoadImageFromMemory(".png",
		(uint8_t *) _binary_resources_spritesheet_bin_start,
		_binary_resources_spritesheet_bin_end - _binary_resources_spritesheet_bin_start
	);
	SPR_load_spritesheet(img_spritesheet);

	// Main Loop
	while (!WindowShouldClose()) {
		TRK_update();

		// DEBUG: Print key presses
		int k = GetKeyPressed();
		if (k != 0) printf("---> Key Pressed: %i\n", k); fflush(stdout);

		if (IsKeyPressed(KEY_FULLSCREEN)) ToggleFullscreen();
		//if (IsKeyPressed(KEY_THEME_1)) g_colour_theme = 0;
		//if (IsKeyPressed(KEY_THEME_2)) g_colour_theme = 1;
		//if (IsKeyPressed(KEY_THEME_3)) g_colour_theme = 2;
		//if (IsKeyPressed(KEY_THEME_4)) g_colour_theme = 3;
		//if (IsKeyPressed(KEY_THEME_5)) g_colour_theme = 4;

		player_ship->vel.x = 0.0f;
		if (IsKeyDown(KEY_LEFT)) player_ship->vel.x = -PLAYER_SPEED;
		if (IsKeyDown(KEY_RIGHT)) player_ship->vel.x = PLAYER_SPEED;

		// Update sprites
		static float anim_timer = 0.0f;
		if (anim_timer <= 0.0f) {
			for (int i=0; i<SPRITE_COUNT; i++) {
				SPR_Sprite s = game_sprites[i];
				game_sprites[i].rect.x += s.vel.x * PIXEL_SIZE;
				game_sprites[i].rect.y += s.vel.y * PIXEL_SIZE;

				game_sprites[i].frame_index++; 
				game_sprites[i].frame_index %= game_sprites[i].frame_count;
			}
			anim_timer = ANIM_SPEED;
		} else {
			anim_timer -= GetFrameTime();
			if (anim_timer < 0.0f) anim_timer = 0.0f;
		}

		// Draw Elements
		BeginDrawing();
		ClearBackground(CLR_BACKGROUND);

		for (int i=0; i<SPRITE_COUNT; i++) {
			SPR_draw(game_sprites[i]);
		}

		EndDrawing();
	}

	// Terminate
	SPR_unload_spritesheet();
	SND_terminate();
	CloseWindow();

	return EXIT_SUCCESS;
}

void print_help() {
	puts("spacies - A simple space-invaders clone");
	puts("");
	puts("Options:");
	puts("  -h,--help		Print this information and exit");
	puts("");
}
