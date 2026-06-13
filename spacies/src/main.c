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

#ifndef VERSION
#define VERSION "x.x.x"
#endif

// Game Definitions
#define FRAMERATE		60		// FPS
#define TICKRATE		4		// TPS
#define PLAYER_SPEED	250.0f	// px/s
#define BULLET_SPEED	600.0f	// px/s
#define MAX_BULLETS		16
#define ALIEN_ROWS		5
#define ALIEN_COLS		11
#define ALIEN_PADDING	10.0f	// px
#define ALIEN_SPEED		5.0f	// px
#define ALIEN_POINTS	100		// pts
#define ALIEN_FIRE_PROB	20		// %
#define MYSTERY_POINTS	1000	// pts
#define MYSTERY_PROB	1		// %
#define PADDING			50.0f	// px
#define SCORE_HEIGHT	100.0f	// px
#define SCORE_PADDING	20.0f	// px

// Sounds
#define SND_WAVE	WAV_SQUARE
#define SND_VOLUME	0.20f		// 10 %
#define SND_DEATH_FREQ	110.0f	// Hz
#define SND_DEATH_TIME	0.150f	// ms (s)
#define SND_FIRE_FREQ	440.0f	// Hz
#define SND_FIRE_TIME	0.050f	// ms (s)

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

typedef uint8_t AlienType;
#define ALIEN_DEAD		(AlienType)(0x00)
#define ALIEN_SMALL		(AlienType)(0x01)
#define ALIEN_MEDIUM	(AlienType)(0x02)
#define ALIEN_LARGE		(AlienType)(0x03)

typedef uint8_t BulletType;
#define BULLET_PLAYER	(BulletType)(0x00)
#define BULLET_ALIEN	(BulletType)(0x01)

typedef struct {
	uint8_t type;
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
int g_score = 0;
int g_lives = 3;

const Color g_colour_themes[5][5] = {
//			Background		Player			Aliens			Text			Mystery Ship
	{	BLACK,		GREEN,		WHITE,		WHITE,		RED			},	// Classic
	{	BLACK,		ORANGE,		ORANGE,		ORANGE,		ORANGE		},	// Orange
	{	DARKBLUE,	BLUE,		BLUE,		BLUE,		BLUE		},	// Commodore
	{	BLACK,		RED,		GREEN,		WHITE,		GREEN		},	// Snake
	{	WHITE,		BLACK,		BLACK,		BLACK,		BLACK		},	// White & Black
};

#define CLR_BACKGROUND (g_colour_themes[g_colour_theme][0])
#define CLR_PLAYER (g_colour_themes[g_colour_theme][1])
#define CLR_ALIENS (g_colour_themes[g_colour_theme][2])
#define CLR_TEXT (g_colour_themes[g_colour_theme][3])
#define CLR_MYSTERY (g_colour_themes[g_colour_theme][4])


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
	SND_volume = SND_VOLUME;

	const Rectangle rec_bullet = {
		.height = 30.0f,
		.width = 5.0f,
	};
	const Rectangle rec_alien = {
		.height = 45.0f,
		.width = 65.0f,
	};
	const Rectangle rec_mystery = {
		.height = 35.0f,
		.width = 95.0f,
	};

	Rectangle player_ship = {
		.height = 30.0f,
		.width = 100.0f,
	};
	player_ship.x = GetScreenWidth() / 2.0f - player_ship.width / 2.0f;
	player_ship.y = GetScreenHeight() - player_ship.height - PADDING;
	int player_fire_cooldown = 2;

	int mystery_counter = 0;
	Rectangle mystery_ship = rec_mystery;

	// Initialise Aliens
	AlienType alien_grid[ALIEN_ROWS][ALIEN_COLS];
	AlienType curr = ALIEN_SMALL;
	for (int row=0; row<ALIEN_ROWS; row++) {
		if (row > 0) curr = ALIEN_MEDIUM;
		if (row > 2) curr = ALIEN_LARGE;
		for (int col=0; col<ALIEN_COLS; col++) alien_grid[row][col] = curr;
	}
	int alien_pos_x = PADDING;
	int alien_pos_y = PADDING + SCORE_HEIGHT;
	int alien_vel_x = ALIEN_SPEED;
	int alien_move_timer = 4;

	// Main Loop
	while (!WindowShouldClose()) {
		TRK_update();

		// Handle Inputs

		//if (IsKeyPressed(INPUT_FULLSCREEN)) ToggleFullscreen();
		if (IsKeyPressed(INPUT_THEME_1)) g_colour_theme = 0;
		if (IsKeyPressed(INPUT_THEME_2)) g_colour_theme = 1;
		if (IsKeyPressed(INPUT_THEME_3)) g_colour_theme = 2;
		if (IsKeyPressed(INPUT_THEME_4)) g_colour_theme = 3;
		if (IsKeyPressed(INPUT_THEME_5)) g_colour_theme = 4;

		if (g_pause_movement == 0.0f) {

			float player_move = PLAYER_SPEED * GetFrameTime();
			if (IsKeyDown(INPUT_LEFT) && player_ship.x > PADDING) {
				player_ship.x -= player_move;
			}
			if (IsKeyDown(INPUT_RIGHT) && player_ship.x < GetScreenWidth() - PADDING - player_ship.width) {
				player_ship.x += player_move;
			}

			if (IsKeyDown(INPUT_FIRE) && player_fire_cooldown <= 0) {
				spawn_bullet(
					player_ship.x + (player_ship.width / 2.0f) - (rec_bullet.width / 2.0f),
					player_ship.y - (rec_bullet.height / 2.0f),
					BULLET_PLAYER
				);
				SND_beep(SND_FIRE_FREQ, SND_FIRE_TIME);
				player_fire_cooldown = 2;
			}


			// Handle Ticks
			static float anim_timer = 0.0f;
			if (anim_timer <= 0.0f) {

				// Move aliens
				if (alien_move_timer <= 0) {
					alien_pos_x += alien_vel_x;

					static const int right_margin = PADDING + ALIEN_COLS * (rec_alien.width + ALIEN_PADDING) - ALIEN_PADDING;
					if (alien_pos_x > GetScreenWidth() - right_margin || alien_pos_x < PADDING) {
						alien_pos_x -= alien_vel_x;
						alien_pos_y += ALIEN_SPEED;
						alien_vel_x = -alien_vel_x;
					}

					alien_move_timer = 4;
				}
				alien_move_timer--;

				// Check whether aliens should fire
				if (GetRandomValue(1, 100) <= ALIEN_FIRE_PROB) {
					int row = GetRandomValue(0, ALIEN_ROWS-1);
					int col = GetRandomValue(0, ALIEN_COLS-1);

					if (alien_grid[row][col] != ALIEN_DEAD) {
						int abx = alien_pos_x + col * (rec_alien.width + ALIEN_PADDING);
						int aby = alien_pos_y + row * (rec_alien.height + ALIEN_PADDING);
						spawn_bullet(abx + rec_alien.width/2.0f, aby, BULLET_ALIEN);
					}
				}

				// Check whether to spawn a mystery ship
				if (mystery_counter == 0 && GetRandomValue(1, 100) <= MYSTERY_PROB) {
					mystery_counter = 1;
					
					mystery_ship.x = 0.0f;
					mystery_ship.y = SCORE_HEIGHT;
				}

				// Handle firing cooldown
				if (player_fire_cooldown > 0) player_fire_cooldown--;

				anim_timer = 1.0f / TICKRATE;
			} else {
				anim_timer -= GetFrameTime();
				if (anim_timer < 0.0f) anim_timer = 0.0f;
			}

			// Move mystery ship
			if (mystery_counter > 0) {
				if (mystery_counter & 0b1) mystery_ship.x += ALIEN_SPEED;
				else mystery_ship.x -= ALIEN_SPEED;

				if (mystery_ship.x < -200.0f || mystery_ship.x + mystery_ship.width > GetScreenWidth() + 200.0f) {
					mystery_counter++;

					if (mystery_counter > 3) mystery_counter = 0;
				}
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

				// Check if we've hit an alien 
				if (b.type == BULLET_PLAYER) {

					if (mystery_counter > 0) {
						if (CheckCollisionRecs(r, mystery_ship)) {
							mystery_counter = 0;
							remove_bullet(i);
							g_score += 1000;
						}
					}

					Rectangle ra = rec_alien;
					int col = (float)(r.x - alien_pos_x) / (float)(ra.width + ALIEN_PADDING);
					int row = (float)(r.y - alien_pos_y) / (float)(ra.height + ALIEN_PADDING);
					if (col < 0 || col >= ALIEN_COLS) continue;
					if (row < 0 || row >= ALIEN_ROWS) continue;

					ra.x = alien_pos_x + col * (ra.width + ALIEN_PADDING);
					ra.y = alien_pos_y + row * (ra.height + ALIEN_PADDING);
					if (alien_grid[row][col] != ALIEN_DEAD && CheckCollisionRecs(r, ra)) {
						alien_grid[row][col] = ALIEN_DEAD;
						remove_bullet(i);
						g_score += 100;
					}

					col++;
					ra.x = alien_pos_x + col * (ra.width + ALIEN_PADDING);
					if (alien_grid[row][col] != ALIEN_DEAD && CheckCollisionRecs(r, ra)) {
						alien_grid[row][col] = ALIEN_DEAD;
						remove_bullet(i);
						g_score += 100;
					}
				}

				// Check if we've hit the player
				if (b.type == BULLET_ALIEN && CheckCollisionRecs(r, player_ship)) {
					// Reset player
					player_ship.x = GetScreenWidth() / 2.0f - player_ship.width / 2.0f;
					player_ship.y = GetScreenHeight() - player_ship.height - PADDING;
					player_fire_cooldown = 2;

					SND_beep(SND_DEATH_FREQ, SND_DEATH_TIME);
					g_pause_movement = 1.0f;

					// TODO: If no lives left; show GAME OVER screen

					remove_bullet(i);
					g_lives--;
				}

			}

		// End of the frame movement
		} else if (g_pause_movement > 0.0f) {
			g_pause_movement -= GetFrameTime();
			if (g_pause_movement < 0.0f) g_pause_movement = 0.0f;
		}


		// Draw Elements

		BeginDrawing();
		ClearBackground(CLR_BACKGROUND);

		// Draw Aliens
		for (int row=0; row<ALIEN_ROWS; row++) {
			for (int col=0; col<ALIEN_COLS; col++) {
				if (alien_grid[row][col] == ALIEN_DEAD) continue;

				Rectangle r = rec_alien;
				r.x = alien_pos_x + col * (r.width + ALIEN_PADDING);
				r.y = alien_pos_y + row * (r.height + ALIEN_PADDING);

				DrawRectangleRec(r, CLR_ALIENS);
			}
		}

		if (mystery_counter > 0) DrawRectangleRec(mystery_ship, CLR_MYSTERY);

		// Draw Bullets
		for (int i=0; i<g_bullet_count; i++) {
			Bullet b = g_bullet_buf[i];
			Rectangle r = rec_bullet;
			r.x = b.pos_x;
			r.y = b.pos_y;

			Color c;
			switch (b.type) {
				default:
				case BULLET_PLAYER: c = CLR_PLAYER; break;
				case BULLET_ALIEN: c = CLR_ALIENS; break;
			}

			DrawRectangleRec(r, c);
		}

		// Draw Player
		DrawRectangleRec(player_ship, CLR_PLAYER);

		// Draw Score
		DrawText(TextFormat("SCORE: %6i", g_score),
			SCORE_PADDING, SCORE_PADDING,
			SCORE_HEIGHT - (2*SCORE_PADDING), CLR_TEXT
		);

		// Draw Lives
		const char *str = TextFormat("LIVES: %i", g_lives);
		const int font_size = SCORE_HEIGHT - (2*SCORE_PADDING);
		DrawText(str,
			GetScreenWidth() - SCORE_PADDING - MeasureText(str, font_size), SCORE_PADDING,
			font_size, CLR_TEXT
		);

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
	g_bullet_buf[bi].type = type;

	switch (type) {
		default:
		case BULLET_PLAYER: g_bullet_buf[bi].vel_y = -BULLET_SPEED; break;
		case BULLET_ALIEN: g_bullet_buf[bi].vel_y = BULLET_SPEED; break;
	}
}

void remove_bullet(int i) {
	if (g_bullet_count <= 0) return;

	g_bullet_count--;
	g_bullet_buf[i] = g_bullet_buf[g_bullet_count];
}

void print_help() {
	puts("spacies - A simple space-invaders clone");
	puts("");
	puts("Version: " VERSION);
	puts("");
	puts("Options:");
	puts("  -h,--help		Print this information and exit");
	puts("");
}
