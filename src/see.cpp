/*
 * see.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: James
 */

#include <cassert>
#include "defs.h"
#include "eval.h"
#include "globals.h"


int32 max(int32 a,int32 b) {
	if (a > b) return a;
	return b;
}

square_t find_least_valuable(position *pos,uint64 attackers) {
	square_t lv_sq = NO_SQUARE;
	int32 lv_score = 0;

	while (attackers) {
		square_t sq = (square_t)get_lsb(attackers);

		int32 my_val = piece_val(pos->piece[sq]);
		if (lv_sq==NO_SQUARE || my_val < lv_score) {
			lv_score = my_val;
			lv_sq = sq;
		}

		attackers ^= bb_squares[sq];
	}

	return lv_sq;
}

void add_xray_attackers(position *pos,square_t from_sq,dir_t dir,
		uint64 *white_attackers,uint64 *black_attackers) {

	assert(dir != NODIR);

	uint64 targets = bb_rays[from_sq][dir];
	uint64 xrays;
	if (dir==NORTH || dir==SOUTH || dir==EAST || dir==WEST) {
		uint64 rook_moves = get_rook_moves(pos,from_sq,targets);
		xrays = rook_moves & (pos->white_rooks | pos->white_queens | pos->black_rooks | pos->black_queens);
	} else {
		uint64 bishop_moves = get_bishop_moves(pos,from_sq,targets);
		xrays = bishop_moves & (pos->white_bishops | pos->white_queens | pos->black_bishops | pos->black_queens);
	}

	assert(get_num_bits(xrays) <= 1);
	if (xrays & pos->white_pieces) {
		*white_attackers |= xrays;
	} else if (xrays & pos->black_pieces) {
		*black_attackers |= xrays;
	}
}

int32 score_promotion(move mv) {
	piece_t promo = get_promopiece(mv);
	if (promo==QUEEN) return 10005;
	if (promo==ROOK) return 10004;
	if (promo==BISHOP) return 10003;
	assert(promo==KNIGHT);
	return 10002;
}

// note mv should already be applied
int32 score_capture(position *pos,move mv) {
	square_t to_sq = get_to_sq(mv);
	square_t from_sq = get_from_sq(mv);
	assert(pos->piece[from_sq]==NO_PIECE);

	// simulate the initial attack
	int32 scores[32];
	scores[0] = piece_val(get_captured_piece(mv));
	int32 scores_ind = 1;

	// play out the sequence
	uint64 white_attackers = get_attackers(pos,to_sq,WHITE);
	uint64 black_attackers = get_attackers(pos,to_sq,BLACK);

	uint64* attackers;

	color_t stm = pos->player;
	square_t current_sq = from_sq;
	piece_t current_piece = (piece_t)pos->piece[to_sq];
	int32 attacked_piece_val = piece_val(current_piece);

	while (true) {
		// xray attackers
		if (current_piece != KNIGHT && current_piece != -KNIGHT
				&& current_piece != KING && current_piece != -KING)
		{
			dir_t dir = direction_tbl[to_sq][current_sq];
			add_xray_attackers(pos,current_sq,dir,&white_attackers,&black_attackers);
		}

		///
		attackers = stm==WHITE ? &white_attackers : &black_attackers;
		current_sq = find_least_valuable(pos,*attackers);
		if (current_sq==NO_SQUARE) break;

		*attackers ^= bb_squares[current_sq];
		current_piece = (piece_t)pos->piece[current_sq];
		assert(current_piece != NO_PIECE);

		scores[scores_ind] = attacked_piece_val - scores[scores_ind-1];
		scores_ind++;
		attacked_piece_val = piece_val(current_piece);
		stm = opposite_player(stm);
	}

	// evaluate the sequence
	while (scores_ind > 1) {
		scores_ind--;
		scores[scores_ind-1] = -max(-scores[scores_ind-1],scores[scores_ind]);
	}

	return scores[0];
}

int32 see(position *pos,move mv) {
	int32 score = 0;

	if (get_promopiece(mv) != NO_PIECE) {
		score = score_promotion(mv);
	}
	if (is_capture(mv)) {
		score += score_capture(pos,mv);
	}

	return score;
}
