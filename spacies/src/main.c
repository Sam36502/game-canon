//	
//		SPACE INVADERS
//	
//	Recreated using Raylib
//	


//	
//	Includes
//	

#include <raylib.h>
#include <stdio.h>
#include <time.h>

#include "../include/beep.h"
#include "../include/track.h"


//	
//	Macro Constants
//	

// Game Definitions
#define FRAMERATE		60		// FPS
#define TICKRATE		4		// TPS
#define PLAYER_SPEED	250.0f	// px/s
#define BULLET_SPEED	600.0f	// px/s
#define MAX_BULLETS		32
#define PADDING			50.0f	// px

// Sounds
#define SND_WAVE WAV_SQUARE

// Inputs
#define INPUT_LEFT			KEY_LEFT
#define INPUT_RIGHT			KEY_RIGHT
#define INPUT_FIRE			KEY_SPACE
#define INPUT_FULLSCREEN	KEY_F
#define INPUT_THEME_1		KEY_ONE
#define INPUT_THEME_2		KEY_TWO
#define INPUT_THEME_3		KEY_THREE
#define INPUT_THEME_4		KEY_FOUR
#define INPUT_THEME_5		KEY_FIVE


//	
//	Type Definitions
//	

typedef struct {
	uint8_t type;	// 0x00 = Player, 0x01 = Alien
	int pos_x;
	int pos_y;
	int vel_y;
} Bullet;


//
//	Function Declarations
//

void spawn_bullet(int x, int y, uint8_t type);
void remove_bullet(int i);
void print_help();

//	
//	Globals
//

static float g_pause_movement = 0.0f;	// How long to pause movement for
static int g_colour_theme = 0;
Bullet g_bullet_buf[MAX_BULLETS];
int g_bullet_count = 0;

const Color g_colour_themes[5][4] = {
//			Background		Player			Aliens			Text
	{	BLACK,		WHITE,		WHITE,		WHITE	},
	{	BLACK,		ORANGE,		ORANGE,		ORANGE	},
	{	DARKBLUE,	BLUE,		BLUE,		BLUE	},
	{	BLACK,		RED,		GREEN,		WHITE	},
	{	WHITE,		BLACK,		BLACK,		BLACK	},
};

#define CLR_BACKGROUND (g_colour_themes[g_colour_theme][0])
#define CLR_PLAYER (g_colour_themes[g_colour_theme][1])
#define CLR_ALIENS (g_colour_themes[g_colour_theme][2])
#define CLR_TEXT (g_colour_themes[g_colour_theme][3])


//	
//	Main Function
//	

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
	//SetTraceLogLevel(LOG_WARNING);	// TODO: Uncomment for PROD
	SetRandomSeed(time(NULL));
	InitWindow(1000, 800,
		"Game Canon - Space Invaders"
	);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(FRAMERATE);

	SND_init();
	SND_loadpreset(SND_WAVE);

	const Rectangle rec_bullet = {
		.height = 40.0f,
		.width = 15.0f,
	};

	Rectangle player_ship = {
		.height = 30.0f,
		.width = 100.0f,
	};
	player_ship.x = GetScreenWidth() / 2.0f - player_ship.width / 2.0f;
	player_ship.y = GetScreenHeight() - player_ship.height - PADDING;

	// Main Loop
	static int frame_counter = 0;
	while (!WindowShouldClose()) {
		TRK_update();

		// Handle Inputs

		//if (IsKeyPressed(INPUT_FULLSCREEN)) ToggleFullscreen();
		if (IsKeyPressed(INPUT_THEME_1)) g_colour_theme = 0;
		if (IsKeyPressed(INPUT_THEME_2)) g_colour_theme = 1;
		if (IsKeyPressed(INPUT_THEME_3)) g_colour_theme = 2;
		if (IsKeyPressed(INPUT_THEME_4)) g_colour_theme = 3;
		if (IsKeyPressed(INPUT_THEME_5)) g_colour_theme = 4;

		float player_move = PLAYER_SPEED * GetFrameTime();
		if (IsKeyDown(INPUT_LEFT) && player_ship.x > PADDING) {
			player_ship.x -= player_move;
		}
		if (IsKeyDown(INPUT_RIGHT) && player_ship.x < GetScreenWidth() - PADDING - player_ship.width) {
			player_ship.x += player_move;
		}

		if (IsKeyPressed(INPUT_FIRE)) {
			spawn_bullet(
				player_ship.x + (player_ship.width / 2.0f) - (rec_bullet.width / 2.0f),
				player_ship.y - (rec_bullet.height / 2.0f),
				0x00
			);
		}


		// Handle Ticks

		static float anim_timer = 0.0f;
		if (anim_timer <= 0.0f) {
			spawn_bullet(
				GetRandomValue(PADDING, GetScreenWidth() - PADDING),
				GetRandomValue(PADDING, GetScreenHeight() - PADDING),
				0x01
			);

			anim_timer = 1.0f / TICKRATE;
		} else {
			anim_timer -= GetFrameTime();
			if (anim_timer < 0.0f) anim_timer = 0.0f;
		}

		// Move Bullets
		for (int i=0; i<g_bullet_count; i++) {
			Bullet b = g_bullet_buf[i];
			g_bullet_buf[i].pos_y += b.vel_y * GetFrameTime();

			Rectangle r = rec_bullet;
			r.x = b.pos_x;
			r.y = b.pos_y;

			// Check if we're off screen
			if (b.pos_y >= GetScreenHeight()
			   || b.pos_y <= 0.0f - rec_bullet.height
			) {
				remove_bullet(i);
				i--; continue;
			}

			// TODO: Handle collisions
		}

		// Draw Elements

		BeginDrawing();
		ClearBackground(CLR_BACKGROUND);

		// Draw Bullets
		for (int i=0; i<g_bullet_count; i++) {
			Bullet b = g_bullet_buf[i];
			Rectangle r = rec_bullet;
			r.x = b.pos_x;
			r.y = b.pos_y;
			Color c = CLR_PLAYER;
			if (b.type == 0x01) c = CLR_ALIENS;
			DrawRectangleRec(r, c);
		}

		// Draw Player
		DrawRectangleRec(player_ship, CLR_PLAYER);

		EndDrawing();
	}

	// Terminate
	SND_terminate();
	CloseWindow();

	return EXIT_SUCCESS;
}


void spawn_bullet(int x, int y, uint8_t type) {
	if (g_bullet_count >= MAX_BULLETS) return;

	int bi = g_bullet_count++;
	g_bullet_buf[bi].pos_x = x;
	g_bullet_buf[bi].pos_y = y;
	g_bullet_buf[bi].type = 0x00;

	if (type == 0x01) g_bullet_buf[bi].vel_y = BULLET_SPEED;
	else g_bullet_buf[bi].vel_y = -BULLET_SPEED;
}

void remove_bullet(int i) {
	if (g_bullet_count <= 0) return;

	g_bullet_count--;
	g_bullet_buf[i] = g_bullet_buf[g_bullet_count];
}

void print_help() {
	puts("spacies - A simple space-invaders clone");
	puts("");
	puts("Options:");
	puts("  -h,--help		Print this information and exit");
	puts("");
}
