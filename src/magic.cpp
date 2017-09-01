/*
 * magic.cpp
 *
 *  Created on: Dec 22, 2016
 *      Author: James
 */

#include <cassert>
#include <cstring>
#include "defs.h"
#include "globals.h"

uint64 bb_rook_masks[64];
uint64 bb_magic_rook_moves[64][4096]; // 4096 = 2^12.  12=max bits for mask
uint64 bb_rook_occ_var[64][4096];
uint64 bb_rook_attack_set[64][4096];
uint64 magic_numbers_rooks[64];
uint32 magic_numbers_shift_rooks[64];

uint64 bb_bishop_masks[64];
uint64 bb_magic_bishop_moves[64][1024]; // 10=max bits for bishop
uint64 bb_bishop_occ_var[64][1024];
uint64 bb_bishop_attack_set[64][1024];
uint64 magic_numbers_bishops[64];
uint32 magic_numbers_shift_bishops[64];


uint64 get_bishop_moves(position* p,square_t from,uint64 targets) {
	assert(p);
	assert(from >= A8 && from <= H1);
	assert(magic_numbers_bishops[from]);
	assert(magic_numbers_shift_bishops[from]);

	uint64 blockers = (p->black_pieces | p->white_pieces) & bb_bishop_masks[from];
	int magic_ind = (int)((blockers * magic_numbers_bishops[from]) >> magic_numbers_shift_bishops[from]);

	return bb_magic_bishop_moves[from][magic_ind] & targets;
}

uint64 get_queen_moves(position* p,square_t from,uint64 targets) {
	return get_bishop_moves(p,from,targets) | get_rook_moves(p,from,targets);
}

uint64 get_rook_moves(position* p,square_t from,uint64 targets) {
	assert(p);
	assert(from >= A8 && from <= H1);
	assert(magic_numbers_rooks[from]);
	assert(magic_numbers_shift_rooks[from]);

	uint64 blockers = (p->black_pieces | p->white_pieces) & bb_rook_masks[from];
	int magic_ind = (int)((blockers * magic_numbers_rooks[from]) >> magic_numbers_shift_rooks[from]);

	return bb_magic_rook_moves[from][magic_ind] & targets;
}

uint64 gen_moves_mask(square_t sq,uint64 occupied,dirFuncType dirFunc) {
	uint64 mask = 0;

	square_t to = dirFunc(sq);
	while (to != NO_SQUARE) {
		mask |= bb_squares[to];
		if (bb_squares[to] & occupied) {
			break;
		}
		to = dirFunc(to);
	}

	return mask;
}

void init_bishop_masks() {
	for (int i=0;i<64;i++) {
		bb_bishop_masks[i] = 0;

		// if on the same diagonal.  "End points" can be ignored.
		for (int k=0;k<64;k++) {
			rank_t my_r = get_rank((square_t)k);
			file_t my_f = get_file((square_t)k);
			if (i != k) {
				if (is_diagonal((square_t)i,(square_t)k)
						&& my_r != RANK_1 && my_r != RANK_8
						&& my_f != FILE_A && my_f != FILE_H) {
					bb_bishop_masks[i] |= bb_squares[k];
				}
			}
		}
	}
}

void init_rook_masks() {

	for (int i=0;i<64;i++) {
		bb_rook_masks[i] = 0;

		rank_t r = get_rank((square_t)i);
		file_t f = get_file((square_t)i);

		// if on the same rank or file.  "End points" can be ignored.
		for (int k=0;k<64;k++) {
			rank_t my_r = get_rank((square_t)k);
			file_t my_f = get_file((square_t)k);
			if (i != k) {
				if ((my_r==r && my_f != FILE_A && my_f != FILE_H)
					|| (my_f==f && my_r != RANK_8 && my_r != RANK_1))
				{
					bb_rook_masks[i] |= bb_squares[k];
				}
			}
		}
	}
}

void init_bishop_occupancy_variations() {
	for (uint32 sq=0;sq<64;sq++) {
		uint64 mask = bb_bishop_masks[sq];
		uint32 num_mask_bits = get_num_bits(mask);
		uint32 num_variations = 1 << num_mask_bits; //pow(2,num_mask_bits);
		assert(num_variations <= 1024);

		for (uint32 i=0;i<num_variations;i++) {
			bb_bishop_occ_var[sq][i] = bb_bishop_attack_set[sq][i] = 0;

			// map the index to an occupancy variation
			int32 index = i;
			while (index) {
				uint32 index_bit = get_lsb(index);
				bb_bishop_occ_var[sq][i] |= isolate_lsb_bit(mask,index_bit);
				index ^= 1 << index_bit;
			}

			// sanity check
			for (uint32 j=0;j<i;j++) {
				assert(bb_bishop_occ_var[sq][i] != bb_bishop_occ_var[sq][j]);
			}

			// create the attack set.  that's the first "blocker" in every direction
			square_t to = northeast((square_t)sq);
			while (to != NO_SQUARE) {
				if (bb_bishop_occ_var[sq][i] & bb_squares[to]) {
					bb_bishop_attack_set[sq][i] |= bb_squares[to];
					break;
				}
				to = northeast(to);
			}
			to = southeast((square_t)sq);
			while (to != NO_SQUARE) {
				if (bb_bishop_occ_var[sq][i] & bb_squares[to]) {
					bb_bishop_attack_set[sq][i] |= bb_squares[to];
					break;
				}
				to = southeast(to);
			}
			to = southwest((square_t)sq);
			while (to != NO_SQUARE) {
				if (bb_bishop_occ_var[sq][i] & bb_squares[to]) {
					bb_bishop_attack_set[sq][i] |= bb_squares[to];
					break;
				}
				to = southwest(to);
			}
			to = northwest((square_t)sq);
			while (to != NO_SQUARE) {
				if (bb_bishop_occ_var[sq][i] & bb_squares[to]) {
					bb_bishop_attack_set[sq][i] |= bb_squares[to];
					break;
				}
				to = northwest(to);
			}
		}
	}
}

void init_rook_occupancy_variations() {
	for (uint32 sq=0;sq<64;sq++) {
		uint64 mask = bb_rook_masks[sq];
		uint32 num_mask_bits = get_num_bits(mask);
		uint32 num_variations = 1 << num_mask_bits; //pow(2,num_mask_bits);
		assert(num_variations <= 4096);

		for (uint32 i=0;i<num_variations;i++) {
			bb_rook_occ_var[sq][i] = bb_rook_attack_set[sq][i] = 0;

			// map the index to an occupancy variation
			int32 index = i;
			while (index) {
				uint32 index_bit = get_lsb(index);
				bb_rook_occ_var[sq][i] |= isolate_lsb_bit(mask,index_bit);
				index ^= 1 << index_bit;
			}

			// sanity check
			for (uint32 j=0;j<i;j++) {
				assert(bb_rook_occ_var[sq][i] != bb_rook_occ_var[sq][j]);
			}

			// create the attack set.  that's the first "blocker" in every direction
			square_t to = north((square_t)sq);
			while (to != NO_SQUARE) {
				if (bb_rook_occ_var[sq][i] & bb_squares[to]) {
					bb_rook_attack_set[sq][i] |= bb_squares[to];
					break;
				}
				to = north(to);
			}
			to = south((square_t)sq);
			while (to != NO_SQUARE) {
				if (bb_rook_occ_var[sq][i] & bb_squares[to]) {
					bb_rook_attack_set[sq][i] |= bb_squares[to];
					break;
				}
				to = south(to);
			}
			to = east((square_t)sq);
			while (to != NO_SQUARE) {
				if (bb_rook_occ_var[sq][i] & bb_squares[to]) {
					bb_rook_attack_set[sq][i] |= bb_squares[to];
					break;
				}
				to = east(to);
			}
			to = west((square_t)sq);
			while (to != NO_SQUARE) {
				if (bb_rook_occ_var[sq][i] & bb_squares[to]) {
					bb_rook_attack_set[sq][i] |= bb_squares[to];
					break;
				}
				to = west(to);
			}
		}
	}
}

void init_magic_numbers(bool is_rook) {
	for (uint32 sq=0;sq<64;sq++) {
		uint64 mask = is_rook ? bb_rook_masks[sq] : bb_bishop_masks[sq];
		uint32 num_mask_bits = get_num_bits(mask);
		uint32 num_variations = 1 << num_mask_bits;

		uint64 magic;
		uint32 magic_shift = 64 - num_mask_bits;

		bool fail;
		bool is_used[4096];
		uint64 used_by[4096];

		do {
			magic = random64() & random64() & random64(); // not many bits set

			for (uint32 i=0;i<num_variations;i++) {
				is_used[i] = false;
			}

			fail = false;
			for (uint32 i=0;i<num_variations;i++) {
				uint64 occupied = is_rook ? bb_rook_occ_var[sq][i] : bb_bishop_occ_var[sq][i];
				int index = (int)((occupied * magic) >> magic_shift);
				assert(index <= (1 << num_mask_bits) && index <= (is_rook ?4096:1024));

				// fail if this index is used by an attack set that is incorrect
				// for this occupancy variation
				uint64 attack_set = is_rook ? bb_rook_attack_set[sq][i] : bb_bishop_attack_set[sq][i];
				fail = is_used[index] && (used_by[index] != attack_set);
				if (fail) {
					break;
				}
				is_used[index] = true;
				used_by[index] = attack_set;
			}
		} while (fail);

		if (is_rook) {
			magic_numbers_rooks[sq] = magic;
			magic_numbers_shift_rooks[sq] = magic_shift;
		} else {
			magic_numbers_bishops[sq] = magic;
			magic_numbers_shift_bishops[sq] = magic_shift;
		}
	}
}

void init_bishop_magic_moves() {
	for (uint32 sq=0;sq<64;sq++) {
		uint64 mask = bb_bishop_masks[sq];
		uint32 num_mask_bits = get_num_bits(mask);
		uint32 num_variations = 1 << num_mask_bits;

		for (uint32 i=0;i<num_variations;i++) {
			int magic_ind = (int)((bb_bishop_occ_var[sq][i] * magic_numbers_bishops[sq]) >> magic_numbers_shift_bishops[sq]);
			bb_magic_bishop_moves[sq][magic_ind] =
					gen_moves_mask((square_t)sq,bb_bishop_occ_var[sq][i],&northeast)
					| gen_moves_mask((square_t)sq,bb_bishop_occ_var[sq][i],&southeast)
					| gen_moves_mask((square_t)sq,bb_bishop_occ_var[sq][i],&southwest)
					| gen_moves_mask((square_t)sq,bb_bishop_occ_var[sq][i],&northwest);
		}
	}
}

void init_rook_magic_moves() {
	for (uint32 sq=0;sq<64;sq++) {
		uint64 mask = bb_rook_masks[sq];
		uint32 num_mask_bits = get_num_bits(mask);
		uint32 num_variations = 1 << num_mask_bits;

		for (uint32 i=0;i<num_variations;i++) {
			int magic_ind = (int)((bb_rook_occ_var[sq][i] * magic_numbers_rooks[sq]) >> magic_numbers_shift_rooks[sq]);
			bb_magic_rook_moves[sq][magic_ind] =
					gen_moves_mask((square_t)sq,bb_rook_occ_var[sq][i],&north)
					| gen_moves_mask((square_t)sq,bb_rook_occ_var[sq][i],&south)
					| gen_moves_mask((square_t)sq,bb_rook_occ_var[sq][i],&east)
					| gen_moves_mask((square_t)sq,bb_rook_occ_var[sq][i],&west);
		}
	}
}

void init_magics() {
	// make sure bitboards are already initialized
	assert(bb_squares[D4]);

	init_rook_masks();
	init_bishop_masks();
	init_rook_occupancy_variations();
	init_bishop_occupancy_variations();
	init_magic_numbers(true); // rooks
	init_magic_numbers(false); // bishops
	init_rook_magic_moves();
	init_bishop_magic_moves();
}




