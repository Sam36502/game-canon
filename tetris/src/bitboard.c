#include "../include/bitboard.h"

//	Shift functions; the opposite edge is filled with 0s

Bitboard bit_shift_left(const Bitboard brd) {
	uint8_t decomp[8] = {
		brd << (8*0 + 1),
		brd << (8*1 + 1),
		brd << (8*2 + 1),
		brd << (8*3 + 1),
		brd << (8*4 + 1),
		brd << (8*5 + 1),
		brd << (8*6 + 1),
		brd << (8*7 + 1),
	};

	uint64_t new = 0;
	new |= decomp[7]; new <<= 8;
	new |= decomp[6]; new <<= 8;
	new |= decomp[5]; new <<= 8;
	new |= decomp[4]; new <<= 8;
	new |= decomp[3]; new <<= 8;
	new |= decomp[2]; new <<= 8;
	new |= decomp[1]; new <<= 8;
	new |= decomp[0];
	return new;
}

Bitboard bit_shift_right(const Bitboard brd);
Bitboard bit_shift_up(const Bitboard brd);
Bitboard bit_shift_down(const Bitboard brd);

//	Rotation

Bitboard bit_rot_clockwise(const Bitboard brd);
Bitboard bit_rot_widdershins(const Bitboard brd);
