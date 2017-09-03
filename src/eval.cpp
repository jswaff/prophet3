/*
 * eval.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: james
 */

#include <cassert>
#include "defs.h"
#include "eval.h"
#include "globals.h"

int32 piece_val(int32 pc) {
	return pvals[pc+6];
}

int32 eval_connected_major_on_7th(position *pos,square_t sq,bool wtm) {
	int32 score = 0;

	uint64 rook_moves = get_rook_moves(pos,sq,bb_rays[sq][EAST]);
	if (wtm) {
		if (rook_moves & (pos->white_rooks | pos->white_queens)) {
			score += connected_majors_on_7th;
		}
	} else {
		if (rook_moves & (pos->black_rooks | pos->black_queens)) {
			score += connected_majors_on_7th;
		}
	}

	return score;
}

int32 eval_major_on_7th(position *pos,square_t sq,bool wtm) {
	int32 score = 0;

	if (wtm) {
		if (get_rank(sq)==RANK_7 && get_rank(pos->black_king)==RANK_8) {
			score += rook_on_7th;
			score += eval_connected_major_on_7th(pos,sq,wtm);
		}
	} else {
		if (get_rank(sq)==RANK_2 && get_rank(pos->white_king)==RANK_1) {
			score += rook_on_7th;
			score += eval_connected_major_on_7th(pos,sq,wtm);
		}
	}

	return score;
}

int32 eval_knights(position *p) {
	int32 score = 0;

	uint64 pmap = p->white_knights;
	while (pmap) {
		square_t sq = (square_t)get_msb(pmap);
		score += eval_knight(p,sq,true);
		score += knight_tropism * distance(sq,p->black_king);
		pmap ^= bb_squares[sq];
	}

	pmap = p->black_knights;
	while (pmap) {
		square_t sq = (square_t)get_lsb(pmap);
		score -= eval_knight(p,sq,false);
		score -= knight_tropism * distance(sq,p->white_king);
		pmap ^= bb_squares[sq];
	}

	return score;
}

int32 eval_knight(position *p,square_t sq,bool wtm) {
	return wtm ? knight_pst[sq] : knight_pst[fliprank[sq]];
}

int32 eval_bishops(position *p) {
	int32 score = 0;

	uint64 pmap = p->white_bishops;
	while (pmap) {
		square_t sq = (square_t)get_msb(pmap);
		score += eval_bishop(p,sq,true);
		pmap ^= bb_squares[sq];
	}

	pmap = p->black_bishops;
	while (pmap) {
		square_t sq = (square_t)get_lsb(pmap);
		score -= eval_bishop(p,sq,false);
		pmap ^= bb_squares[sq];
	}

	return score;
}

int32 eval_bishop(position *p,square_t sq,bool wtm) {
	return wtm ? bishop_pst[sq] : bishop_pst[fliprank[sq]];
}

int32 eval_rook_open_file(position *p,square_t sq,bool wtm) {
	int32 score = 0;

	uint64 friends,enemies;
	if (wtm) {
		friends = p->white_pawns;
		enemies = p->black_pawns;
	} else {
		friends = p->black_pawns;
		enemies = p->white_pawns;
	}

	uint64 file_mask = bb_files[get_file(sq)] ^ bb_squares[sq];
	if (!(file_mask & friends)) {
		if (file_mask & enemies) {
			score += rook_half_open_file;
		} else {
			score += rook_open_file;
		}
	}

	return score;
}

int32 eval_rooks(position *p) {
	int32 score = 0;

	uint64 pmap = p->white_rooks;
	while (pmap) {
		square_t sq = (square_t)get_msb(pmap);
		score += eval_rook(p,sq,true);
		pmap ^= bb_squares[sq];
	}

	pmap = p->black_rooks;
	while (pmap) {
		square_t sq = (square_t)get_lsb(pmap);
		score -= eval_rook(p,sq,false);
		pmap ^= bb_squares[sq];
	}

	return score;
}

int32 eval_rook(position *p,square_t sq,bool wtm) {
	int32 score = wtm ? rook_pst[sq] : rook_pst[fliprank[sq]];
	score += eval_major_on_7th(p,sq,wtm);
	score += eval_rook_open_file(p,sq,wtm);
	return score;
}

int32 eval_queens(position *p) {
	int32 score = 0;

	uint64 pmap = p->white_queens;
	while (pmap) {
		square_t sq = (square_t)get_msb(pmap);
		score += eval_queen(p,sq,true);
		pmap ^= bb_squares[sq];
	}

	pmap = p->black_queens;
	while (pmap) {
		square_t sq = (square_t)get_lsb(pmap);
		score -= eval_queen(p,sq,false);
		pmap ^= bb_squares[sq];
	}

	return score;
}

int32 eval_queen(position *p,square_t sq,bool wtm) {
	int32 score = wtm ? queen_pst[sq] : queen_pst[fliprank[sq]];
	score += eval_major_on_7th(p,sq,wtm);

	return score;
}

int32 eval_kings(position *p,int32_pair npmat) {
	int32 score = 0;

	const int endgame_threshold = knight_val*2 + rook_val;

	// if black has a lot of material then eval white in the middle game
	if (npmat.val2 >= endgame_threshold) {
		score += king_pst[p->white_king];
		score += eval_scale(eval_king_safety(p,true),npmat.val2);
	} else {
		score += king_endgame_pst[p->white_king];
	}

	if (npmat.val1 >= endgame_threshold) {
		score -= king_pst[fliprank[p->black_king]];
		score -= eval_scale(eval_king_safety(p,false),npmat.val1);
	} else {
		score -= king_endgame_pst[fliprank[p->black_king]];
	}

	return score;
}

int32 eval_pawns(position *p) {
	int32 score = 0;

	uint64 pmap = p->white_pawns;
	while (pmap) {
		square_t sq = (square_t)get_msb(pmap);
		score += eval_pawn(p,sq,true);
		pmap ^= bb_squares[sq];
	}

	pmap = p->black_pawns;
	while (pmap) {
		square_t sq = (square_t)get_lsb(pmap);
		score -= eval_pawn(p,sq,false);
		pmap ^= bb_squares[sq];
	}

	return score;
}


int32_pair eval_nonpawn_material_slow(position *p) {
	int32_pair score;
	score.val1 = 0;
	score.val2 = 0;

	for (int sq=0;sq<64;sq++) {
		switch(p->piece[sq]) {
		case KNIGHT:
			score.val1 += knight_val;
			break;
		case -KNIGHT:
			score.val2 += knight_val;
			break;
		case BISHOP:
			score.val1 += bishop_val;
			break;
		case -BISHOP:
			score.val2 += bishop_val;
			break;
		case ROOK:
			score.val1 += rook_val;
			break;
		case -ROOK:
			score.val2 += rook_val;
			break;
		case QUEEN:
			score.val1 += queen_val;
			break;
		case -QUEEN:
			score.val2 += queen_val;
			break;
		}
	}

	return score;
}

int32_pair eval_nonpawn_material(position *p) {
	int32_pair score;

	score.val1 = p->piece_counts[WHITE][ROOK] * rook_val
			+ p->piece_counts[WHITE][KNIGHT] * knight_val
			+ p->piece_counts[WHITE][BISHOP] * bishop_val
			+ p->piece_counts[WHITE][QUEEN] * queen_val;

	score.val2 = p->piece_counts[BLACK][ROOK] * rook_val
			+ p->piece_counts[BLACK][KNIGHT] * knight_val
			+ p->piece_counts[BLACK][BISHOP] * bishop_val
			+ p->piece_counts[BLACK][QUEEN] * queen_val;

	assert(score.val1==eval_nonpawn_material_slow(p).val1);
	assert(score.val2==eval_nonpawn_material_slow(p).val2);


	return score;
}

int32_pair eval_pawn_material_slow(position *p) {
	int32_pair score;
	score.val1 = 0;
	score.val2 = 0;

	for (int sq=0;sq<64;sq++) {
		switch(p->piece[sq]) {
		case PAWN:
			score.val1 += pawn_val;
			break;
		case -PAWN:
			score.val2 += pawn_val;
			break;
		}
	}

	return score;
}

int32_pair eval_pawn_material(position *p) {
	int32_pair score;
	score.val1 = p->piece_counts[WHITE][PAWN] * pawn_val;
	score.val2 = p->piece_counts[BLACK][PAWN] * pawn_val;

	assert(score.val1==eval_pawn_material_slow(p).val1);
	assert(score.val2==eval_pawn_material_slow(p).val2);

	return score;
}

int32 eval(position *p,bool mat_only) {
	int32 score = 0;

	int32_pair npmat = eval_nonpawn_material(p);
	score += npmat.val1 - npmat.val2;

	int32_pair pmat = eval_pawn_material(p);
	score += pmat.val1 - pmat.val2;

	if (!mat_only) {
		score += eval_pawns(p);
		score += eval_knights(p);
		score += eval_bishops(p);
		score += eval_rooks(p);
		score += eval_queens(p);
		score += eval_kings(p,npmat);
	}

	return p->player==WHITE ? score : -score;
}

int32 eval_scale(int32 score,int32 material) {
	return score * material / all_nonpawn_pieces_val;
}

