/*
 * bitboards.cpp
 *
 *  Created on: Apr 20, 2016
 *      Author: James
 */

#include <cassert>
#include "defs.h"
#include "globals.h"

uint32 lsb[65536];
uint32 msb[65536];

uint32 get_num_bits(uint64 val) {
	uint32 n = 0;

	// Brian Kernighan's approach
	while (val) {
		n++;
		val &= val-1;
	}

	return n;
}

uint32 get_lsb_slow(uint64 val) {
	assert(val > 0);

	for (int i=0;i<64;i++) {
		if (val & bb_squares[i]) {
			return i;
		}
	}

	return 0;
}

uint32 get_lsb(uint64 val) {
	assert(val > 0);

	if (val & 65535) {
		return lsb[(uint32)(val & 65535)];
	}

	val >>= 16;
	if (val & 65535) {
		return lsb[(uint32)(val & 65535)] + 16;
	}

	val >>= 16;
	if (val & 65535) {
		return lsb[(uint32)(val & 65535)] + 32;
	}

	val >>= 16;
	return lsb[(uint32)val] + 48;
}

uint32 get_msb_slow(uint64 val) {
	assert(val > 0);

	for (int i=63;i>=0;i--) {
		if (val & bb_squares[i]) {
			return i;
		}
	}

	return 0;
}

uint32 get_msb(uint64 val) {
	assert(val > 0);

	if ((val >> 48) & 65535) {
		return msb[(uint32)((val>>48)&65535)] + 48;
	}

	if ((val >> 32) & 65535) {
		return msb[(uint32)((val>>32)&65535)] + 32;
	}

	if ((val >> 16) & 65535) {
		return msb[(uint32)((val>>16)&65535)] + 16;
	}

	return msb[(uint32)val];
}

uint64 isolate_lsb_bit(uint64 mask,uint32 bit_pos) {

	uint32 n=0;
	for (int i=0;i<64;i++) {
		if (bb_squares[i] & mask) {
			if (n==bit_pos) {
				return bb_squares[i];
			}
			n++;
		}
	}

	return 0;
}

void init_knight_moves() {

	for (int i=0;i<64;i++) {
		square_t sq = (square_t)i;
		bb_knight_moves[sq] = 0;
		rank_t r = get_rank(sq);
		file_t f = get_file(sq);
		if (f > FILE_A) {
			if (r > RANK_7) {
				bb_knight_moves[sq] |= bb_squares[north(northwest(sq))];
			}
			if (r < RANK_2) {
				bb_knight_moves[sq] |= bb_squares[south(southwest(sq))];
			}
		}
		if (f > FILE_B) {
			if (r > RANK_8) {
				bb_knight_moves[sq] |= bb_squares[northwest(west(sq))];
			}
			if (r < RANK_1) {
				bb_knight_moves[sq] |= bb_squares[southwest(west(sq))];
			}
		}
		if (f < FILE_G) {
			if (r > RANK_8) {
				bb_knight_moves[sq] |= bb_squares[northeast(east(sq))];
			}
			if (r < RANK_1) {
				bb_knight_moves[sq] |= bb_squares[southeast(east(sq))];
			}
		}
		if (f < FILE_H) {
			if (r > RANK_7) {
				bb_knight_moves[sq] |= bb_squares[north(northeast(sq))];
			}
			if (r < RANK_2) {
				bb_knight_moves[sq] |= bb_squares[south(southeast(sq))];
			}
		}
	}
}

void init_king_moves() {
	for (int i=0;i<64;i++) {
		square_t sq = (square_t)i;
		bb_king_moves[sq] = 0;
		rank_t r = get_rank(sq);
		file_t f = get_file(sq);
		if (r > RANK_8) {
			if (f > FILE_A) {
				bb_king_moves[sq] |= bb_squares[northwest(sq)];
			}
			bb_king_moves[sq] |= bb_squares[north(sq)];
			if (f < FILE_H) {
				bb_king_moves[sq] |= bb_squares[northeast(sq)];
			}
		}
		if (f < FILE_H) {
			bb_king_moves[sq] |= bb_squares[east(sq)];
		}
		if (r < RANK_1) {
			if (f < FILE_H) {
				bb_king_moves[sq] |= bb_squares[southeast(sq)];
			}
			bb_king_moves[sq] |= bb_squares[south(sq)];
			if (f > FILE_A) {
				bb_king_moves[sq] |= bb_squares[southwest(sq)];
			}
		}
		if (f > FILE_A) {
			bb_king_moves[sq] |= bb_squares[west(sq)];
		}
	}
}

void init_rays() {
	for (int i=0;i<64;i++) {
		file_t file_i = get_file((square_t)i);
		rank_t rank_i = get_rank((square_t)i);

		for (int j=0;j<8;j++) {
			bb_rays[i][j] = 0;
		}

		for (int j=0;j<64;j++) {
			file_t file_j = get_file((square_t)j);
			rank_t rank_j = get_rank((square_t)j);

			// figure out if j is north or south of i
			if (file_i==file_j) {
				if (j<i) {
					bb_rays[i][NORTH] |= bb_squares[j];
				} else if (j>i) {
					bb_rays[i][SOUTH] |= bb_squares[j];
				}
			}
			// figure out if j is east or west of i
			if (rank_i==rank_j) {
				if (j<i) {
					bb_rays[i][WEST] |= bb_squares[j];
				} else if (j>i) {
					bb_rays[i][EAST] |= bb_squares[j];
				}
			}

			if (is_diagonal((square_t)i,(square_t)j)) {
				if (rank_j < rank_i) {
					if (file_j < file_i) {
						bb_rays[i][NORTHWEST] |= bb_squares[j];
					} else if (file_j > file_i) {
						bb_rays[i][NORTHEAST] |= bb_squares[j];
					}
				} else if (rank_j > rank_i) {
					if (file_j < file_i) {
						bb_rays[i][SOUTHWEST] |= bb_squares[j];
					} else if (file_j > file_i) {
						bb_rays[i][SOUTHEAST] |= bb_squares[j];
					}
				}
			}
		}
	}
}

void init_pawn_attacks() {
	for (int i=0;i<64;i++) {
		square_t sq = (square_t)i;
		file_t file_i = get_file(sq);
		rank_t rank_i = get_rank(sq);

		bb_pawn_attacks[i][BLACK] = bb_pawn_attacks[i][WHITE] = 0;
		if (rank_i != RANK_1 && rank_i != RANK_8) {
			if (file_i > FILE_A) {
				bb_pawn_attacks[i][WHITE] |= bb_squares[northwest(sq)];
				bb_pawn_attacks[i][BLACK] |= bb_squares[southwest(sq)];
			}
			if (file_i < FILE_H) {
				bb_pawn_attacks[i][WHITE] |= bb_squares[northeast(sq)];
				bb_pawn_attacks[i][BLACK] |= bb_squares[southeast(sq)];
			}
		}
	}
}

void init_bitboards() {

	for (int i=0;i<64;i++) {
		bb_squares[i] = ((uint64)1) << i;
	}

	for (int i=0;i<8;i++) {
		bb_ranks[i] = 0;
		bb_files[i] = 0;
	}

	for (int i=0;i<64;i++) {
		bb_ranks[get_rank((square_t)i)] |= bb_squares[i];
		bb_files[get_file((square_t)i)] |= bb_squares[i];
	}

	lsb[0] = 0; msb[0] = 0;
	for (uint32 i=1;i<65536;i++) {
		lsb[i] = get_lsb_slow(i);
		msb[i] = get_msb_slow(i);
	}

	init_knight_moves();
	init_king_moves();
	init_pawn_attacks();
	init_rays();
}

