#include "../include/tetris.h"

const uint16_t g_tetromino_bitmaps[7] = {
	(
	   0b0100 << 12
	 | 0b0100 << 8
	 | 0b0100 << 4
	 | 0b0100 << 0
	),
	(
	   0b0000 << 12
	 | 0b0110 << 8
	 | 0b0110 << 4
	 | 0b0000 << 0
	),
	(
	   0b0100 << 12
	 | 0b0110 << 8
	 | 0b0100 << 4
	 | 0b0000 << 0
	),
	(
	   0b0100 << 12
	 | 0b0100 << 8
	 | 0b0110 << 4
	 | 0b0000 << 0
	),
	(
	   0b0010 << 12
	 | 0b0010 << 8
	 | 0b0110 << 4
	 | 0b0000 << 0
	),
	(
	   0b0000 << 12
	 | 0b0000 << 8
	 | 0b0000 << 4
	 | 0b0000 << 0
	),
	(
	   0b0000 << 12
	 | 0b0000 << 8
	 | 0b0000 << 4
	 | 0b0000 << 0
	),
};

uint8_t g_board[BOARD_HEIGHT][BOARD_WIDTH];


void clear_board() {
	for (int y=0; y<BOARD_HEIGHT; y++) {
		for (int x=0; x<BOARD_WIDTH; x++) {
			g_board[y][x] = 0;
		}
	}
}

void draw_tetromino(Tetromino tet, Color palette[], int num_clrs) {
	if (tet.colour == 0 || tet.colour >= num_clrs) return;
	const int px_sz = GetRenderHeight() / BOARD_HEIGHT;
	const float offset_x = (float)(GetRenderWidth()) / 2 - ((float) BOARD_WIDTH * px_sz) / 2;
	const float offset_y = (float)(GetRenderHeight()) / 2 - ((float) BOARD_HEIGHT * px_sz) / 2;

	for (int y=0; y<4; y++) {
		uint8_t row = (tet.bitmap >> (4*y)) & 0b1111;
		for (int x=0; x<4; x++) {
			if ((row >> x & 0b1) == 0) continue;

			DrawRectangle(
				offset_x + px_sz * (tet.pos_x + x),
				offset_y + px_sz * (tet.pos_y + y),
				px_sz, px_sz, palette[tet.colour]
			);
		}
	}

}

void draw_board(Color bg_clr, Color palette[], int num_clrs) {
	const int px_sz = GetRenderHeight() / BOARD_HEIGHT;
	const float offset_x = (float)(GetRenderWidth()) / 2 - ((float) BOARD_WIDTH * px_sz) / 2;
	const float offset_y = (float)(GetRenderHeight()) / 2 - ((float) BOARD_HEIGHT * px_sz) / 2;

	// Clear background
	DrawRectangle(
		offset_x, offset_y,
		px_sz * BOARD_WIDTH, px_sz * BOARD_HEIGHT,
		bg_clr
	);

	// Draw board contents
	for (int y=0; y<BOARD_HEIGHT; y++) {
		for (int x=0; x<BOARD_WIDTH; x++) {

			uint8_t n = g_board[y][x];
			if (n == 0 || n >= num_clrs) continue;
			DrawRectangle(
				offset_x + px_sz * x,
				offset_y + px_sz * y,
				px_sz, px_sz, palette[n]
			);

		}
	}
}


void check_line(int y) {
}
