//	
//		PONG
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

#define SCREEN_WIDTH GetRenderWidth()
#define SCREEN_HEIGHT GetRenderHeight()
#define FRAMERATE 60

// Sounds
#define SND_1_PITCH 256.0f	// Hz
#define SND_2_PITCH 512.0f	// Hz
#define SND_LENGTH 0.100f	// s
#define SND_WAVE WAV_TRIANGLE

// Inputs
#define KEY_P1_UP 87
#define KEY_P1_DN 83
#define KEY_P2_UP 265
#define KEY_P2_DN 264
#define KEY_FULLSCREEN 70
#define KEY_THEME_1 49
#define KEY_THEME_2 50
#define KEY_THEME_3 51
#define KEY_THEME_4 52
#define KEY_THEME_5 53

// Game Constants
#define BALL_SPEED 8
#define BALL_SIZE 25
#define PADDLE_SPEED 8
#define PADDLE_SIZE 150
#define PLAYER1_X BALL_SIZE
#define PLAYER2_X (SCREEN_WIDTH - 2*BALL_SIZE)
#define DIGIT_SIZE 10

// Function Declarations
void reset_ball(Vector2 *pos, Vector2 *vel);
void draw_score(int x, int n);
void play_jingle();
void print_help();

// Globals
static float g_pause_movement = 0.0f;	// How long to pause movement for
static int g_num_players = 1;
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

		// Check for number of players	
		char *tail;
		long n = strtol(argv[1], &tail, 10);
		if (*tail != '\0') {
			printf("Invalid player-number argument '%s'\n", argv[1]);
			print_help();
			return EXIT_FAILURE;
		}

		if (n < 0 || n > 2) {
			printf("Invalid player-number argument '%s'; Must be between 0 and 2\n", argv[1]);
			print_help();
			return EXIT_FAILURE;
		}

		g_num_players = (int) n;
	}

	// Initialise
	SetTraceLogLevel(LOG_NONE);
	SetRandomSeed(time(NULL));
	InitWindow(1000, 800,
		"Game Canon - Pong"
	);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(FRAMERATE);

	SND_init();
	SND_loadpreset(SND_WAVE);

	// Set up game elements
	Vector2 ball_pos;
	Vector2 ball_vel;
	reset_ball(&ball_pos, &ball_vel);

	int player1_pos = SCREEN_HEIGHT / 2.0 - PADDLE_SIZE / 2.0;
	int player1_score = 0;

	int player2_pos = SCREEN_HEIGHT / 2.0 - PADDLE_SIZE / 2.0;
	int player2_score = 0;

	// Main Loop
	play_jingle();
	while (!WindowShouldClose()) {
		TRK_update();

		// DEBUG: Print key presses
		//int k = GetKeyPressed();
		//if (k != 0) printf("---> Key Pressed: %i\n", k); fflush(stdout);

		if (IsKeyPressed(KEY_FULLSCREEN)) ToggleFullscreen();
		if (IsKeyPressed(KEY_THEME_1)) g_colour_theme = 0;
		if (IsKeyPressed(KEY_THEME_2)) g_colour_theme = 1;
		if (IsKeyPressed(KEY_THEME_3)) g_colour_theme = 2;
		if (IsKeyPressed(KEY_THEME_4)) g_colour_theme = 3;
		if (IsKeyPressed(KEY_THEME_5)) g_colour_theme = 4;

		// Update positions
		if (g_pause_movement <= 0.0f) {
			ball_pos.x += ball_vel.x;
			ball_pos.y += ball_vel.y;

			if (IsWindowResized()) {
				if (ball_pos.x < 0) ball_pos.x = 0;
				if (ball_pos.x > SCREEN_WIDTH - BALL_SIZE) ball_pos.x = SCREEN_WIDTH - BALL_SIZE;
				if (ball_pos.y < 0) ball_pos.y = 0;
				if (ball_pos.y > SCREEN_HEIGHT - BALL_SIZE) ball_pos.y = SCREEN_HEIGHT - BALL_SIZE;
				if (player1_pos < 0) player1_pos = 0;
				if (player1_pos > SCREEN_HEIGHT - PADDLE_SIZE) player1_pos = SCREEN_HEIGHT - PADDLE_SIZE;
				if (player2_pos < 0) player2_pos = 0;
				if (player2_pos > SCREEN_HEIGHT - PADDLE_SIZE) player2_pos = SCREEN_HEIGHT - PADDLE_SIZE;
			}

			int player1_speed = 0;
			if (g_num_players >= 1) {
				if (IsKeyDown(KEY_P1_UP) && player1_pos > 0) player1_speed = -PADDLE_SPEED;
				if (IsKeyDown(KEY_P1_DN) && player1_pos < SCREEN_HEIGHT - PADDLE_SIZE) player1_speed = PADDLE_SPEED;
			} else {
				if (ball_vel.x < 0.0f && ball_pos.y < player1_pos) player1_speed = -PADDLE_SPEED;
				if (ball_vel.x < 0.0f && ball_pos.y > player1_pos + PADDLE_SIZE) player1_speed = PADDLE_SPEED;
			}

			int player2_speed = 0;
			if (g_num_players >= 2) {
				if (IsKeyDown(KEY_P2_UP) && player2_pos > 0) player2_speed = -PADDLE_SPEED;
				if (IsKeyDown(KEY_P2_DN) && player2_pos < SCREEN_HEIGHT - PADDLE_SIZE) player2_speed = PADDLE_SPEED;
			} else {
				if (ball_vel.x > 0.0f && ball_pos.y < player2_pos) player2_speed = -PADDLE_SPEED;
				if (ball_vel.x > 0.0f && ball_pos.y > player2_pos + PADDLE_SIZE) player2_speed = PADDLE_SPEED;
			}

			player1_pos += player1_speed;
			player2_pos += player2_speed;

			// Reflect off top and bottom
			if (ball_pos.y < 0) {
				ball_pos.y = 0;
				ball_vel.y = -ball_vel.y;
				SND_beep(SND_1_PITCH, SND_LENGTH);
			}
			if (ball_pos.y > SCREEN_HEIGHT - BALL_SIZE) {
				ball_pos.y = SCREEN_HEIGHT - BALL_SIZE;
				ball_vel.y = -ball_vel.y;
				SND_beep(SND_1_PITCH, SND_LENGTH);
			}

			// Reflect off Paddles
			if (ball_pos.x < PLAYER1_X + BALL_SIZE
				&& ball_pos.y + BALL_SIZE >= player1_pos
				&& ball_pos.y <= player1_pos + PADDLE_SIZE
			) {
				ball_pos.x = PLAYER1_X + BALL_SIZE;
				ball_vel.x = -ball_vel.x;
				ball_vel.y += (float) player1_speed / 2.0f;
				if (g_num_players < 1 && fabs(ball_vel.y) < BALL_SPEED/2.0f) ball_vel.y *= GetRandomValue(105, 115) / 100.0f;
				SND_beep(SND_1_PITCH, SND_LENGTH);
			}

			if (ball_pos.x + BALL_SIZE > PLAYER2_X
				&& ball_pos.y + BALL_SIZE >= player2_pos
				&& ball_pos.y <= player2_pos + PADDLE_SIZE
			) {
				ball_pos.x = PLAYER2_X - BALL_SIZE;
				ball_vel.x = -ball_vel.x;
				ball_vel.y += (float) player2_speed / 2.0f;
				if (g_num_players < 2 && fabs(ball_vel.y) < BALL_SPEED/2.0f) ball_vel.y *= GetRandomValue(105, 115) / 100.0f;
				SND_beep(SND_1_PITCH, SND_LENGTH);
			}


			// Detect "goals"
			if (ball_pos.x < PLAYER1_X) {
				if (player2_score < 9) {
					SND_beep(SND_2_PITCH, SND_LENGTH);
					player2_score++;
				} else {
					play_jingle();
					player1_score = 0;
					player2_score = 0;
				}
				reset_ball(&ball_pos, &ball_vel);
			}

			if (ball_pos.x > PLAYER2_X) {
				if (player1_score < 9) {
					SND_beep(SND_2_PITCH, SND_LENGTH);
					player1_score++;
				} else {
					play_jingle();
					player1_score = 0;
					player2_score = 0;
				}
				reset_ball(&ball_pos, &ball_vel);
			}

		} else {
			// Progress pause timer
			g_pause_movement -= GetFrameTime();
			if (g_pause_movement <= 0.0f) {
				g_pause_movement = 0.0f;
				TRK_queue_stop();
			}
		}

		// Draw Elements
		BeginDrawing();
		ClearBackground(CLR_BACKGROUND);

		// Ball
		DrawRectangle(
			ball_pos.x, ball_pos.y,
			BALL_SIZE, BALL_SIZE,
			CLR_BALL
		);

		// Player 1 Paddle
		DrawRectangle(
			PLAYER1_X, player1_pos,
			BALL_SIZE, PADDLE_SIZE,
			CLR_PADDLE
		);

		// Player 2 Paddle
		DrawRectangle(
			PLAYER2_X, player2_pos,
			BALL_SIZE, PADDLE_SIZE,
			CLR_PADDLE
		);

		// Scores
		const int third = SCREEN_WIDTH / 3;
		draw_score(third * 1 - (DIGIT_SIZE / 2), player1_score);
		draw_score(third * 2 - (DIGIT_SIZE / 2), player2_score);

		EndDrawing();
	}

	// Terminate
	SND_terminate();
	CloseWindow();

	return EXIT_SUCCESS;
}

void reset_ball(Vector2 *pos, Vector2 *vel) {
	*pos = (Vector2){
		SCREEN_WIDTH / 2.0,
		SCREEN_HEIGHT / 2.0,
	};

	float angle = (GetRandomValue(0, 6282) - 3141) / 2000.0f;
	if (angle < 0.0f) angle -= 3.141f / 2.0f;
	angle -= 3.141 / 4.0f;
	*vel = Vector2Rotate(
		(Vector2){ BALL_SPEED, 0.0f },
		angle
	);
	
	// Prevent perfectly horizontal balls (leads to game getting stuck in AI vs AI games)
	if (fabs(vel->y) < 0.01f) vel->y += (GetRandomValue(0, 1)==0 ? 1 : -1) * BALL_SPEED / 2.0f;

	g_pause_movement = 0.500f;
}

// Draws a score digit to the screen with large, blocky digits
void draw_score(int x, int n) {

	n %= 10;
	static uint8_t _bit_pattern[10] = {
		[0] = 0b01110111,
		[1] = 0b00010010,
		[2] = 0b01011101,
		[3] = 0b01011011,
		[4] = 0b00111010,
		[5] = 0b01101011,
		[6] = 0b00101111,
		[7] = 0b01010010,
		[8] = 0b01111111,
		[9] = 0b01111010,
	};

	uint8_t bits = _bit_pattern[n];
	int len = DIGIT_SIZE * 3;
	if ((bits >> 6 & 0b1) == 1)
		DrawRectangle( x + (0 * len), DIGIT_SIZE + (0 * len), len + DIGIT_SIZE, DIGIT_SIZE, CLR_DIGIT);
	if ((bits >> 5 & 0b1) == 1)
		DrawRectangle( x + (0 * len), DIGIT_SIZE + (0 * len), DIGIT_SIZE, len + DIGIT_SIZE, CLR_DIGIT);
	if ((bits >> 4 & 0b1) == 1)
		DrawRectangle( x + (1 * len), DIGIT_SIZE + (0 * len), DIGIT_SIZE, len + DIGIT_SIZE, CLR_DIGIT);
	if ((bits >> 3 & 0b1) == 1)
		DrawRectangle( x + (0 * len), DIGIT_SIZE + (1 * len), len + DIGIT_SIZE, DIGIT_SIZE, CLR_DIGIT);
	if ((bits >> 2 & 0b1) == 1)
		DrawRectangle( x + (0 * len), DIGIT_SIZE + (1 * len), DIGIT_SIZE, len + DIGIT_SIZE, CLR_DIGIT);
	if ((bits >> 1 & 0b1) == 1)
		DrawRectangle( x + (1 * len), DIGIT_SIZE + (1 * len), DIGIT_SIZE, len + DIGIT_SIZE, CLR_DIGIT);
	if ((bits >> 0 & 0b1) == 1)
		DrawRectangle( x + (0 * len), DIGIT_SIZE + (2 * len), len + DIGIT_SIZE, DIGIT_SIZE, CLR_DIGIT);

}

void play_jingle() {
	g_pause_movement = 1.00f; // 1 seconds
	TRK_queue_note(261.63f, 0.100f); // C-4
	TRK_queue_note(329.63f, 0.100f); // E-4
	TRK_queue_note(392.00f, 0.100f); // G-4
	TRK_queue_note(523.25f, 0.200f); // C-5
	TRK_queue_play();
}

void print_help() {
	puts("pong - A simple pong game");
	puts("");
	puts("Options:");
	puts("  -h,--help		Print this information and exit");
	puts("");
	puts("The first argument lets you set the number of players:");
	puts("  0 - Both players are AI-controlled");
	puts("  1 - Play against an AI (default)");
	puts("  2 - Both paddles are controllable");
}
