#ifndef BITBOARD_H
#define BITBOARD_H

//	
//	Functions for handling binary matrices
//	

#include <stdint.h>

//	Type Definitions
typedef uint64_t Bitboard;


//	Function Declarations
//	Returns a new bitboard with the shift applied; leaves the original untouched

//	Shift functions; the opposite edge is filled with 0s
Bitboard bit_shift_left(const Bitboard brd);
Bitboard bit_shift_right(const Bitboard brd);
Bitboard bit_shift_up(const Bitboard brd);
Bitboard bit_shift_down(const Bitboard brd);

//	Rotation
Bitboard bit_rot_clockwise(const Bitboard brd);
Bitboard bit_rot_widdershins(const Bitboard brd);

#endif
