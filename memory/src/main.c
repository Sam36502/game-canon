//	
//		Memory Game
//	
//	  Made with Raylib
//	
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "../include/beep.h"

#define FRAMERATE 60
#define ANIM_SPEED 0.500f
#define GRID_SIZE 2
#define MAX_PLAYERS 10
#define MAX_NAME_LEN 6

// Sounds
#define SND_WAVE WAV_SQUARE

// Layout Constants
#define BOX_TITLE		(Rectangle){ 0.05f, 0.05f, 0.50f, 0.10f }
#define BOX_PROMPT		(Rectangle){ 0.05f, 0.90f, 0.50f, 0.05f }
#define BOX_SCORE		(Rectangle){ 0.65f, 0.05f, 0.30f, 0.10f }
#define BOX_TEXT		(Rectangle){ 0.10f, 0.20f, 0.50f, 0.60f }
#define BOX_CARD_GRID	(Rectangle){ 0.35f, 0.20f, 0.60f, 0.60f }
#define CARD_SIZE 		(GetRenderHeight() * BOX_CARD_GRID.height / GRID_SIZE)
#define CARD_PADDING 	5 // px
#define scale_w(x)		(int)(x * GetRenderWidth())
#define scale_h(y) 		(int)(y * GetRenderHeight())

// Function Declarations
int hovered_card();
void draw_card(int x, int y, bool is_uncovered, bool outline);
void print_help();

// Globals
static int g_colour_theme = 0;
static uint8_t g_card_grid[GRID_SIZE][GRID_SIZE];
static int g_scoreboard[MAX_PLAYERS];
static char g_names[MAX_PLAYERS][MAX_NAME_LEN];

const Color g_colour_themes[][4] = {
//			Background		Text			Card Back		Card Outline
	{	SKYBLUE,	BLACK,		WHITE,		RED		},
	{	BLACK,		ORANGE,		YELLOW,		ORANGE	},
	{	DARKBLUE,	BLUE,		DARKBLUE,	BLUE	},
	{	BLACK,		RED,		GREEN,		WHITE	},
	{	WHITE,		BLACK,		WHITE,		BLACK	},
};
#define CLR_BACKGROUND (g_colour_themes[g_colour_theme][0])
#define CLR_TEXT (g_colour_themes[g_colour_theme][1])
#define CLR_CARD_BG (g_colour_themes[g_colour_theme][2])
#define CLR_CARD_FG (g_colour_themes[g_colour_theme][3])


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
	//SetTraceLogLevel(LOG_WARNING); // TODO: Reenable for release
	SetRandomSeed(time(NULL));
	InitWindow(1000, 800,
		"Memory"
	);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(FRAMERATE);

	SND_init();
	SND_loadpreset(SND_WAVE);

	// Set up game variables
	enum {
		STATE_SETUP_PLAYERS,
		STATE_PLAY,
		STATE_RESULTS,
		STATE_HIGHSCORES,
	} game_state = STATE_SETUP_PLAYERS;
	int player_count = 1;
	int curr_player = 1;
	int name_index = 0;
	bool is_editing = false;
	bool prompt_to_continue = true;

	int first_choice = -1;
	int second_choice = -1;
	bool is_match = false;

	// Initialise random grid of cards
	int deck_size = (GRID_SIZE * GRID_SIZE);
	int card_count = deck_size / 2;
	int card_deck[deck_size];
	int remaining_cards = deck_size;
	for (int i=0; i<card_count; i++) {
		card_deck[i] = i;
		card_deck[i + card_count] = i;
	}

	for (int y=0; y<GRID_SIZE; y++) {
		for (int x=0; x<GRID_SIZE; x++) {
			int ri = GetRandomValue(0, deck_size-1);
			g_card_grid[y][x] = card_deck[ri];

			card_deck[ri] = card_deck[deck_size-1];
			deck_size--;
		}
	}

	// Initialise player names
	for (int i=0; i<MAX_PLAYERS; i++) {
		g_names[i][0] = 'P';
		g_names[i][1] = 'L';
		g_names[i][2] = 'R';
		g_names[i][3] = ' ';
		g_names[i][4] = '0' + i;
	}

	int curr_streak = 1;

	Rectangle rect_btn_dec = {
		10, 10, 50, 50
	};
	Rectangle rect_btn_inc = {
		80, 10, 50, 50
	};

	// Main Loop
	static int frame_counter = 0;
	while (!WindowShouldClose()) {

		// Handle Input

		if (!is_editing) {
			if (IsKeyPressed(KEY_F)) ToggleFullscreen();
			if (IsKeyPressed(KEY_ONE)) g_colour_theme = 0;
			if (IsKeyPressed(KEY_TWO)) g_colour_theme = 1;
			if (IsKeyPressed(KEY_THREE)) g_colour_theme = 2;
			if (IsKeyPressed(KEY_FOUR)) g_colour_theme = 3;
			if (IsKeyPressed(KEY_FIVE)) g_colour_theme = 4;
		}

		switch (game_state) {

			case STATE_SETUP_PLAYERS: {

				bool should_dec = IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressedRepeat(KEY_KP_SUBTRACT);
				should_dec |= IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), rect_btn_dec);
				if (should_dec && player_count > 1) player_count--;

				bool should_inc = IsKeyPressed(KEY_KP_ADD) || IsKeyPressedRepeat(KEY_KP_ADD);
				should_inc |= IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), rect_btn_inc);
				if (should_inc && player_count < MAX_PLAYERS) player_count++;

				if (IsKeyPressed(KEY_UP)) {
					name_index = 0;
					if (is_editing && curr_player > 1) curr_player--;
					else curr_player = 1;
					is_editing = true;
				}

				if (IsKeyPressed(KEY_DOWN)) {
					name_index = 0;
					curr_player++;
					if (is_editing && curr_player < player_count) curr_player++;
					else curr_player = player_count;
					is_editing = true;
				}

				if (!is_editing && IsKeyPressed(KEY_SPACE)) {
					game_state = STATE_PLAY;
					prompt_to_continue = false;
					curr_player = 1;
					for (int i=0; i<player_count; i++) g_scoreboard[i] = 0;
				}

				if (is_editing) {
					int typed = GetCharPressed();
					while (typed > 0) {
						//printf("Typed '%c' (0x%04X); %s Alpha-Numeric\n", typed, typed, isalnum(typed)?"IS":"IS NOT");
						if (isalnum(typed) || isspace(typed)) {
							g_names[curr_player-1][name_index++] = toupper(typed);
							if (name_index >= MAX_NAME_LEN-1) is_editing = false;
						}

						typed = GetCharPressed();
					}
				}
			}; break;

			case STATE_PLAY: {
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					int clicked = hovered_card();

					if (clicked >= 0 && first_choice < 0) {
						first_choice = clicked;
					} else if (clicked >= 0 && clicked != first_choice && second_choice < 0) {
						second_choice = clicked;
						prompt_to_continue = true;
					}

				}

				if (prompt_to_continue && IsKeyPressed(KEY_SPACE)) {
					if (is_match) {
						int y = first_choice / GRID_SIZE;
						int x = first_choice % GRID_SIZE;
						g_card_grid[y][x] |= 0x80;
						y = second_choice / GRID_SIZE;
						x = second_choice % GRID_SIZE;
						g_card_grid[y][x] |= 0x80;
						remaining_cards -= 2;

						g_scoreboard[curr_player - 1] += curr_streak;
						curr_streak++;
					} else {
						curr_player++;
						if (curr_player > player_count) curr_player = 1;
						curr_streak = 1;
					}

					prompt_to_continue = false;
					first_choice = -1;
					second_choice = -1;
					is_match = false;

					if (remaining_cards <= 0) game_state = STATE_RESULTS;
				}
			}; break;

			case STATE_RESULTS: {
			}; break;

			case STATE_HIGHSCORES: {
			}; break;

		}

		// Draw Elements

		BeginDrawing();
		ClearBackground(CLR_BACKGROUND);

		// Draw Title
		DrawText("Memory Game",
			scale_w(BOX_TITLE.x),
			scale_h(BOX_TITLE.y),
			50, CLR_TEXT
		);

		switch (game_state) {

			case STATE_SETUP_PLAYERS: {
				DrawText("Set up Players:",
					scale_w(BOX_TITLE.x),
					scale_h(BOX_TITLE.y) + 70 + 10,
					30, CLR_TEXT
				);

				for (int i=0; i<player_count; i++) {
					DrawRectangle(
						scale_w(BOX_TITLE.x) + 40,
						scale_h(BOX_TITLE.y) + 120 + 10 + i*40,
						104, 34,
						CLR_CARD_BG
					);

					if (i+1 == curr_player && is_editing) {
						DrawRectangleLinesEx(
							(Rectangle){
								scale_w(BOX_TITLE.x) - 2 + 40,
								scale_h(BOX_TITLE.y) - 2 + 120 + 10 + i*40,
								104 + 4, 34 + 4,
							},
							2.0f, CLR_TEXT
						);
						DrawRectangle(
							scale_w(BOX_TITLE.x) + 2 + 40 + name_index * 20,
							scale_h(BOX_TITLE.y) + 2 + 120 + 10 + i*40,
							20, 30, LIGHTGRAY
						);
					}

					DrawText(TextFormat("% 2i:", i+1),
						scale_w(BOX_TITLE.x) + 2,
						scale_h(BOX_TITLE.y) + 2 + 120 + 10 + i*40,
						30, CLR_TEXT
					);
					for (int j=0; j<MAX_NAME_LEN; j++) {
						DrawText(TextFormat("%c", g_names[i][j]),
							scale_w(BOX_TITLE.x) + 2 + 40 + j * 20,
							scale_h(BOX_TITLE.y) + 2 + 120 + 10 + i*40,
							30, CLR_TEXT
						);
					}
				}

				DrawRectangleRec(rect_btn_dec, RED);
				DrawRectangleRec(rect_btn_inc, GREEN);
			}; break;

			case STATE_PLAY: {
				DrawText(TextFormat("%s, it's your turn:", g_names[curr_player]),
					scale_w(BOX_TITLE.x),
					scale_h(BOX_TITLE.y) + 70 + 10,
					30, CLR_TEXT
				);

				int hovered = hovered_card();
				int index = 0;
				int first_card = 0;
				int second_card = 0;
				for (int y=0; y<GRID_SIZE; y++) {
					for (int x=0; x<GRID_SIZE; x++) {
						if (index == first_choice) first_card = g_card_grid[y][x];
						if (index == second_choice) second_card = g_card_grid[y][x];

						bool is_uncovered = (index == first_choice || index == second_choice);
						bool outline = (index == hovered) || is_uncovered;
						draw_card(x, y, is_uncovered, outline);

						index++;
					}
				}

				is_match = first_card == second_card;
				if (prompt_to_continue && is_match) {
					DrawText("Match!",
						scale_w(BOX_PROMPT.x),
						scale_h(BOX_PROMPT.y) - 60,
						30, CLR_TEXT
					);
				}
			}; break;

			case STATE_RESULTS: {
				for (int y=0; y<GRID_SIZE; y++) {
					for (int x=0; x<GRID_SIZE; x++) {
						g_card_grid[y][x] &= 0x7F;
						draw_card(x, y, true, true);
					}
				}
			}; break;

			case STATE_HIGHSCORES: {
			}; break;

		}

		// Draw continue prompt
		if (prompt_to_continue) {
			DrawText("Press space to continue...",
				scale_w(BOX_PROMPT.x),
				scale_h(BOX_PROMPT.y) - 30,
				scale_h(BOX_PROMPT.height), CLR_TEXT
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


int hovered_card() {
	Vector2 mouse = GetMousePosition();
	int x = (mouse.x - scale_w(BOX_CARD_GRID.x)) / CARD_SIZE;
	int y = (mouse.y - scale_h(BOX_CARD_GRID.y)) / CARD_SIZE;

	if (g_card_grid[y][x] >= 0x80) {
		return -1;
	}

	if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) {
		return -1;
	}

	return x + y * GRID_SIZE;
}

void draw_card(int x, int y, bool is_uncovered, bool outline) {
	if (g_card_grid[y][x] >= 0x80) return;
	uint8_t card = g_card_grid[y][x] & 0x7F;

	const float sz = CARD_SIZE - CARD_PADDING;
	Vector2 pos = {
		.x = scale_w(BOX_CARD_GRID.x) + (x * CARD_SIZE),
		.y = scale_h(BOX_CARD_GRID.y) + (y * CARD_SIZE),
	};

	DrawRectangleV(pos, (Vector2){ sz, sz }, CLR_CARD_BG);

	if (is_uncovered) {
		DrawText(TextFormat("%i", card),
			(int)(pos.x + 10),
			(int)(pos.y + 10),
			30, CLR_TEXT
		);
	} else {
		DrawRectangleLinesEx((Rectangle){
				pos.x + 5, pos.y + 5,
				sz - 10, sz - 10,
			},
			2.0f, CLR_CARD_FG
		);
	}

	if (outline) {
		DrawRectangleLinesEx((Rectangle){
				pos.x - 2, pos.y - 2,
				sz + 4, sz + 4,
			},
			2.0f, CLR_TEXT
		);
	}

}

void print_help() {
	puts("memory - A basic multiplayer memory game");
	puts("");
	puts("Options:");
	puts("  -h,--help		Print this information and exit");
	puts("");
}
