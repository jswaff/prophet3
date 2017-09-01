/*
 * next.cpp
 *
 *  Created on: Feb 10, 2016
 *      Author: James
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "globals.h"
#include "defs.h"

bool test_good_move(move mv,position *pos);

bool next(position *pos,move **m,move_order_dto *mo) {
	assert(mo->currentp >= mo->startp);

	if (mo->next_stage==PVMOVE) {
		mo->next_stage = HASHMV;

		assert(mo->currentp==mo->startp);
		assert(mo->endp==mo->startp);

		if (good_move(pos,mo->pv_move)) {
			assert(test_good_move(mo->pv_move,pos));
			*m = &mo->pv_move;
			return true;
		}
	}

	if (mo->next_stage==HASHMV) {
		mo->next_stage = GENCAPS;

		assert(mo->hash_move==clear_score(mo->hash_move));
		assert(mo->pv_move==clear_score(mo->pv_move));

		if (mo->hash_move != mo->pv_move && good_move(pos,mo->hash_move)) {
			assert(test_good_move(mo->hash_move,pos));
			*m = &mo->hash_move;
			return true;
		}
	}

	if (mo->next_stage==GENCAPS) {
		mo->next_stage = CAPTURES_PROMOS;


		assert(mo->currentp==mo->startp);
		assert(mo->endp==mo->startp);

		mo->endp = gen_moves(mo->startp,pos,true,false);
		for (move* mp=mo->startp;mp<mo->endp;mp++) {
			if (*mp==mo->pv_move || *mp==mo->hash_move) {
				*mp = 0;
			} else {
				int32 score = mvvlva(pos,*mp);
				assert(score >= -20000 && score <= 20000);
				set_move_score(mp,score);
			}
		}
	}

	// captures promotions
	if (mo->next_stage==CAPTURES_PROMOS) {
		if (best_at_top(mo->currentp,mo->endp)) {
			if (get_move_score(mo->currentp) > 0) { // only caps and promos should have a score >0
				assert(is_capture(*mo->currentp) || get_promopiece(*mo->currentp)!=NO_PIECE);
				*m = mo->currentp;
				mo->currentp++;
				return true;
			}
		}
		mo->next_stage = KILLER1;
	}

	if (mo->caps_only==false) {
		// killer1
		if (mo->next_stage==KILLER1) {
			mo->next_stage = KILLER2;

			if (mo->killer1 != mo->pv_move && mo->killer1 != mo->hash_move
					&& good_move(pos,mo->killer1)) {
				assert(test_good_move(mo->killer1,pos));
				assert(!is_capture(mo->killer1));
				assert(mo->killer1==clear_score(mo->killer1));
				*m = &mo->killer1;
				return true;
			}
		}

		// killer2
		if (mo->next_stage==KILLER2) {
			mo->next_stage = GENNONCAPS;

			if (mo->killer2 != mo->killer1 && mo->killer2 != mo->pv_move
					&& mo->killer2 != mo->hash_move	&& good_move(pos,mo->killer2)) {
				assert(test_good_move(mo->killer2,pos));
				assert(!is_capture(mo->killer2));
				assert(mo->killer2==clear_score(mo->killer2));
				*m = &mo->killer2;
				return true;
			}
		}

		if (mo->next_stage==GENNONCAPS) {
			mo->next_stage = REMAINING;

			assert(mo->currentp >= mo->startp);
			assert(mo->currentp <= mo->endp);

			move* mystartp = mo->endp;
			mo->endp = gen_moves(mystartp,pos,false,true);
			for (move* mp=mystartp;mp<mo->endp;mp++) {
				if (*mp==mo->pv_move || *mp==mo->hash_move || *mp==mo->killer1 || *mp==mo->killer2) {
					*mp = 0;
				}
			}
		}
	}

	// play them as they come
	while (mo->currentp < mo->endp){
		*m = mo->currentp;
		mo->currentp++;
		if (**m != 0) {
			return true;
		}
	}

	return false;
}

bool best_at_top(move *startp,move *endp) {

	move *bestp = 0;
	int32 bestscore = 0;

	for (move *mp=startp;mp<endp;mp++) {
		if (*mp != 0) {
			int32 score = get_move_score(mp);
			if (bestp==0 || score > bestscore) {
				bestp = mp;
				bestscore = score;
			}
		}
	}

	if (bestp) {
		move tmpmv = *startp;
		*startp = *bestp;
		*bestp = tmpmv;
		return true;
	}

	return false;
}

bool place_move_on_top(move *start,move *end,move top_move) {
	assert(start);
	assert(end);

	if (top_move==BADMOVE) return false;

	assert(clear_score(top_move)==top_move);

	bool found_move=false;
	for (move *mp=start;mp<end;mp++) {
		if (*mp == top_move) {
			move tmp_move = *start;
			*start = top_move;
			*mp = tmp_move;
			found_move=true;
			break;
		}
	}
	return found_move;
}

bool good_pawn_move(position *p,move m) {

	square_t from_sq = get_from_sq(m);
	square_t to_sq = get_to_sq(m);
	piece_t captured_piece = get_captured_piece(m);

	if (captured_piece==NO_PIECE) {
		if (p->piece[to_sq] != NO_PIECE) return false;
		if (p->player==WHITE) {
			square_t nsq = north(from_sq);
			square_t nnsq = north(nsq);
			if (to_sq==nsq || (to_sq==nnsq && p->piece[nsq]==NO_PIECE)) return true;
		} else {
			square_t ssq = south(from_sq);
			square_t sssq = south(ssq);
			if (to_sq==ssq || (to_sq==sssq && p->piece[ssq]==NO_PIECE)) return true;
		}
	} else {
		if (!(bb_pawn_attacks[from_sq][p->player] & bb_squares[to_sq])) return false;
		if (p->player==WHITE) {
			if (is_epcapture(m)) {
				if (to_sq==p->ep_sq && p->piece[south(to_sq)]==-PAWN) return true;
			} else {
				if (p->piece[to_sq]==-captured_piece) return true;
			}
		} else {
			if (is_epcapture(m)) {
				if (to_sq==p->ep_sq && p->piece[north(to_sq)]==PAWN) return true;
			} else {
				if (p->piece[to_sq]==captured_piece) return true;
			}
		}
	}

	return false;
}

bool good_move(position *p,move m) {
	if (m==BADMOVE) return false;

	square_t from_sq = get_from_sq(m);
	square_t to_sq = get_to_sq(m);
	piece_t mover = get_piece(m);
	piece_t captured_piece = get_captured_piece(m);

	if (from_sq < A8 || from_sq > H1 || to_sq < A8 || to_sq > H1) return false;

	// is the piece the correct color for the player on move?
	int32 piece_on_board = p->piece[from_sq];
	if (p->player==WHITE) {
		if (!is_white_piece(piece_on_board)) return false;
	} else {
		if (!is_black_piece(piece_on_board)) return false;
	}

	// is the piece on the from square correct?
	if (p->player==WHITE) {
		if (mover != piece_on_board) return false;
	} else {
		if (mover != -piece_on_board) return false;
	}

	if (mover==PAWN) {
		return good_pawn_move(p,m);
	}

	// validate captured piece is right color
	if (captured_piece != NO_PIECE) {
		if (p->player==WHITE) {
			if (is_not_black_piece(p->piece[to_sq])) return false;
		} else {
			if (is_not_white_piece(p->piece[to_sq])) return false;
		}
	} else {
		if (p->piece[to_sq] != NO_PIECE) return false;
	}

	if (mover==KNIGHT) {
		if (bb_knight_moves[from_sq] & bb_squares[to_sq]) return true;
	} else if (mover==BISHOP) {
		uint64 targets = p->player==WHITE ? ~p->white_pieces : ~p->black_pieces;
		if (get_bishop_moves(p,from_sq,targets) & bb_squares[to_sq]) return true;
	} else if (mover==ROOK) {
		uint64 targets = p->player==WHITE ? ~p->white_pieces : ~p->black_pieces;
		if (get_rook_moves(p,from_sq,targets) & bb_squares[to_sq]) return true;
	} else if (mover==QUEEN) {
		uint64 targets = p->player==WHITE ? ~p->white_pieces : ~p->black_pieces;
		if (get_queen_moves(p,from_sq,targets) & bb_squares[to_sq]) return true;
	} else if (mover==KING) {
		if (bb_king_moves[from_sq] & bb_squares[to_sq]) return true;
		if (is_castle(m)) {
			if (to_sq==G1 && can_castle_wk(p)
					&& p->piece[F1]==NO_PIECE && p->piece[G1]==NO_PIECE
					&& !attacked(p,E1,BLACK) && !attacked(p,F1,BLACK)) {
				return true;
			} else if (to_sq==C1 && can_castle_wq(p)
					&& p->piece[D1]==NO_PIECE && p->piece[C1]==NO_PIECE && p->piece[B1]==NO_PIECE
					&& !attacked(p,E1,BLACK) && !attacked(p,D1,BLACK)) {
				return true;
			} else if (to_sq==G8 && can_castle_bk(p) && p->piece[F8]==NO_PIECE
					&& p->piece[G8]==NO_PIECE
					&& !attacked(p,E8,WHITE) && !attacked(p,F8,WHITE)) {
				return true;
			} else if (to_sq==C8 && can_castle_bq(p)
					&& p->piece[D8]==NO_PIECE && p->piece[C8]==NO_PIECE && p->piece[B8]==NO_PIECE
					&& !attacked(p,E8,WHITE) && !attacked(p,D8,WHITE)) {
				return true;
			}
		}
	}

	return false;
}

// for testing
bool test_good_move(move mv,position *pos) {
	move moves[MAX_MOVES_PER_PLY];
	move *endp = gen_moves(moves,pos,true,true);

	bool valid =  is_in_move_list(mv,moves,endp);
	if (!valid) {
		char mv_text[10];
		move_to_str(mv,mv_text);
		print("good_move says move %s is good.\n",mv_text);
		draw_board(pos);
	}

	return valid;
}

