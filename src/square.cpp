/*
 * square.cpp
 *
 *  Created on: Dec 20, 2011
 *      Author: james
 */


#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "defs.h"
#include "globals.h"


dir_t get_dir_slow(square_t from,square_t to) {
	int32 f_diff = get_file(from) - get_file(to);
	int32 r_diff = get_rank(from) - get_rank(to);

	if (f_diff == 0) {
		// same file
		if (r_diff < 0) return SOUTH;
		if (r_diff > 0) return NORTH;
	} else if (f_diff < 0) {
		// from is west of to
		if (r_diff == 0) return EAST;
		if (r_diff == f_diff) return SOUTHEAST;
		if (r_diff == -f_diff) return NORTHEAST;
	} else { // f_diff > 0
		// from is east of to
		if (r_diff == 0) return WEST;
		if (r_diff == f_diff) return NORTHWEST;
		if (r_diff == -f_diff) return SOUTHWEST;
	}

	return NODIR;
}


rank_t get_rank(square_t sq) {
	assert(sq >= A8 && sq <= H1);
	return (rank_t)(sq >> 3);
}

file_t get_file(square_t sq) {
	assert(sq >= A8 && sq <= H1);
	return (file_t)(sq & 7);
}

square_t get_square(rank_t rank,file_t file) {
	assert(rank >= RANK_8 && rank <= RANK_1);
	assert(file >= FILE_A && file <= FILE_H);
	return square_t(rank*8+file);
}

int32 rank_distance(square_t sq1,square_t sq2) {
	assert(sq1 >= A8 && sq1 <= H1);
	assert(sq2 >= A8 && sq2 <= H1);
	return std::abs(get_rank(sq1)-get_rank(sq2));
}

int32 file_distance(square_t sq1,square_t sq2) {
	assert(sq1 >= A8 && sq1 <= H1);
	assert(sq2 >= A8 && sq2 <= H1);
	return std::abs(get_file(sq1)-get_file(sq2));
}

int32 distance(square_t sq1,square_t sq2) {
	assert(sq1 >= A8 && sq1 <= H1);
	assert(sq2 >= A8 && sq2 <= H1);
	//	return abs(RANK(sq1)-RANK(sq2)) + abs(FILE(sq1)-FILE(sq2));
	return  0;
}

square_t north(square_t sq) {
	assert((sq >= A8 && sq <= H1) || sq == NO_SQUARE);
	if (sq==NO_SQUARE || get_rank(sq)==RANK_8) return NO_SQUARE;
	return (square_t)(sq-8);
}

square_t south(square_t sq) {
	assert((sq >= A8 && sq <= H1) || sq == NO_SQUARE);
	if (sq==NO_SQUARE || get_rank(sq)==RANK_1) return NO_SQUARE;
	return (square_t)(sq+8);
}

square_t east(square_t sq) {
	assert((sq >= A8 && sq <= H1) || sq == NO_SQUARE);
	if (sq==NO_SQUARE || get_file(sq)==FILE_H) return NO_SQUARE;
	return (square_t)(sq+1);
}

square_t west(square_t sq) {
	assert((sq >= A8 && sq <= H1) || sq == NO_SQUARE);
	if (sq==NO_SQUARE || get_file(sq)==FILE_A) return NO_SQUARE;
	return (square_t)(sq-1);
}

square_t northeast(square_t sq) {
	assert((sq >= A8 && sq <= H1) || sq == NO_SQUARE);
	if (sq==NO_SQUARE || get_rank(sq)==RANK_8 || get_file(sq)==FILE_H) return NO_SQUARE;
	return (square_t)(sq-7);
}

square_t southeast(square_t sq) {
	assert((sq >= A8 && sq <= H1) || sq == NO_SQUARE);
	if (sq==NO_SQUARE || get_rank(sq)==RANK_1 || get_file(sq)==FILE_H) return NO_SQUARE;
	return (square_t)(sq+9);
}

square_t southwest(square_t sq) {
	assert((sq >= A8 && sq <= H1) || sq == NO_SQUARE);
	if (sq==NO_SQUARE || get_rank(sq)==RANK_1 || get_file(sq)==FILE_A) return NO_SQUARE;
	return (square_t)(sq+7);
}

square_t northwest(square_t sq) {
	assert((sq >= A8 && sq <= H1) || sq == NO_SQUARE);
	if (sq==NO_SQUARE || get_rank(sq)==RANK_8 || get_file(sq)==FILE_A) return NO_SQUARE;
	return (square_t)(sq-9);
}

bool is_light_sq(square_t sq) {
	assert(sq>=A8 && sq<=H1);
	rank_t r = get_rank(sq);
	file_t f = get_file(sq);
	return (r%2) == (f%2);
}

bool is_dark_sq(square_t sq) {
	assert(sq>=A8 && sq<=H1);
	return !is_light_sq(sq);
}

bool is_diagonal(square_t sq1,square_t sq2) {
	return rank_distance(sq1,sq2)==file_distance(sq1,sq2);
}

void init_directions() {
	for (int i=0;i<64;i++) {
		for (int j=0;j<64;j++) {
			direction_tbl[i][j] = get_dir_slow((square_t)i,(square_t)j);
		}
	}
}
