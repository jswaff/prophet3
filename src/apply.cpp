/*
 * apply.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: james
 */

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "defs.h"
#include "globals.h"

piece_t remove_captured_piece(position* p,move m) {
	assert(is_capture(m));
	piece_t captured;
	if (is_epcapture(m)) {
		// remove pawn
		if (p->player==WHITE) { // black WAS on move
			captured = remove_piece(p,north(p->ep_sq));
			assert(captured==PAWN);
		} else {
			captured = remove_piece(p,south(p->ep_sq));
			assert(captured==-PAWN);
		}
	} else {
		captured = remove_piece(p,get_to_sq(m));
	}

	assert(captured != NO_PIECE);
	return captured;
}

/**
 * Add the moving piece to the destination square.
 * If the moving piece is a pawn, the fifty move counter is reset, and the ep target square is
 * set if appropriate.  If the moving piece is a king, the king square is updated.  If it's a
 * castle, the rook is moved.
 */
void add_piece_to_destination(position* p,move m) {
	square_t from_sq=get_from_sq(m),to_sq=get_to_sq(m);
	int32 piece = p->piece[from_sq];
	add_piece(p,piece,to_sq);
	switch (piece) {
	case PAWN:
		p->fifty_counter=0;
		if (to_sq==north(north(from_sq))) {
			p->ep_sq=north(from_sq);
		} else if (get_rank(to_sq)==RANK_8) {
			assert(get_promopiece(m) != NO_PIECE);
			remove_piece(p,to_sq);
			add_piece(p,get_promopiece(m),to_sq);
		}
		break;
	case -PAWN:
		p->fifty_counter=0;
		if (to_sq==south(south(from_sq))) {
			p->ep_sq=south(from_sq);
		} else if (get_rank(to_sq)==RANK_1) {
			assert(get_promopiece(m) != NO_PIECE);
			remove_piece(p,to_sq);
			add_piece(p,-get_promopiece(m),to_sq);
		}
		break;
	case KING:
		p->white_king=to_sq;
		// move rook if this is a castle
		if (from_sq==E1) {
			if (to_sq==G1) {
				assert(is_castle(m));
				p->fifty_counter=0;
				remove_piece(p,H1);
				add_piece(p,ROOK,F1);
			} else if (to_sq==C1) {
				assert(is_castle(m));
				p->fifty_counter=0;
				remove_piece(p,A1);
				add_piece(p,ROOK,D1);
			}
		}
		break;
	case -KING:
		p->black_king=to_sq;
		if (from_sq==E8) {
			if (to_sq==G8) {
				assert(is_castle(m));
				p->fifty_counter=0;
				remove_piece(p,H8);
				add_piece(p,-ROOK,F8);
			} else if (to_sq==C8) {
				assert(is_castle(m));
				p->fifty_counter=0;
				remove_piece(p,A8);
				add_piece(p,-ROOK,D8);
			}
		}
		break;
	}
}

void apply_move(position *p,move m,undo *undo_stack) {
	assert(p);
	assert(clear_score(m));
	assert(test_pos_consistency(p));
	assert(undo_stack);

	// capture information to undo move
	undo *u = undo_stack+p->move_counter;
	u->mv = m;
	u->hash_key = p->hash_key;
	u->ep_sq = p->ep_sq;
	u->fifty_counter = p->fifty_counter;
	u->castling_rights = p->castling_rights;

	// clear current values
	p->hash_key ^= zkeys.ptm[p->player];
	p->hash_key ^= zkeys.ep[p->ep_sq];
	p->hash_key ^= zkeys.casting_rights[p->castling_rights];

	p->player = opposite_player(p->player);
	p->move_counter++;
	p->fifty_counter++;  // may get reset

	if (is_capture(m)) {
		p->fifty_counter=0;
		u->captured = remove_captured_piece(p,m);
	} else {
		u->captured = NO_PIECE;
	}
	assert(abs(u->captured)==get_captured_piece(m));

	// this could get reset
	p->ep_sq=NO_SQUARE;

	add_piece_to_destination(p,m);
	remove_castling_availability(p,m);
	remove_piece(p,get_from_sq(m));

	// add in current values
	p->hash_key ^= zkeys.ptm[p->player];
	p->hash_key ^= zkeys.ep[p->ep_sq];
	p->hash_key ^= zkeys.casting_rights[p->castling_rights];

	assert(p->hash_key==build_hash_key(p));
	assert(test_pos_consistency(p));
}

void undo_last_move(position *p,undo *undo_stack) {
	assert(p);
	assert(p->move_counter > 0);
	assert(undo_stack);

	undo *u = undo_stack + p->move_counter - 1;

	p->player = opposite_player(p->player);
	p->ep_sq = u->ep_sq;
	p->castling_rights = u->castling_rights;
	p->fifty_counter = u->fifty_counter;
	p->move_counter--;

	move mv = u->mv;
	assert(abs(u->captured)==get_captured_piece(mv));

	// remove the moving piece from to_sq
	square_t to_sq = get_to_sq(mv);
	square_t from_sq = get_from_sq(mv);
	piece_t piece = get_piece(mv);

	if (p->piece[to_sq] != NO_PIECE) {
		remove_piece(p,to_sq);
	}

	// place the moving piece on from_sq
	add_piece(p,piece * (p->player==WHITE ? 1 : -1),from_sq);
	if (piece==KING) {
		if (p->player==WHITE) {
			p->white_king = from_sq;
		} else {
			p->black_king = from_sq;
		}
	}

	// restore the captured piece
	if (is_capture(mv)) {
		if (is_epcapture(mv)) {
			if (p->player==BLACK) {
				add_piece(p,PAWN,(square_t)(p->ep_sq-8));
			} else {
				add_piece(p,-PAWN,(square_t)(p->ep_sq+8));
			}
		} else {
			add_piece(p,u->captured,to_sq);
		}
	} else if (is_castle(mv)) {
		if (from_sq==E1) {
			if (to_sq==G1) {
				remove_piece(p,F1);
				add_piece(p,ROOK,H1);
			} else {
				remove_piece(p,D1);
				add_piece(p,ROOK,A1);
			}
		} else {
			if (to_sq==G8) {
				remove_piece(p,F8);
				add_piece(p,-ROOK,H8);
			} else {
				remove_piece(p,D8);
				add_piece(p,-ROOK,A8);
			}
		}
	}

	p->hash_key = u->hash_key;
}

square_t apply_null_move(position *p) {
	square_t ep_sq = p->ep_sq;

	// clear current values
	p->hash_key ^= zkeys.ptm[p->player];
	p->hash_key ^= zkeys.ep[p->ep_sq];

	// change player and reset EP square
	p->player = opposite_player(p->player);
	p->ep_sq = NO_SQUARE;

	// set current values
	p->hash_key ^= zkeys.ep[p->ep_sq];
	p->hash_key ^= zkeys.ptm[p->player];

	return ep_sq;
}

void undo_null_move(position *p,square_t ep_sq) {
	// clear current values
	p->hash_key ^= zkeys.ptm[p->player];
	p->hash_key ^= zkeys.ep[p->ep_sq];

	// change player and reset EP square
	p->player = opposite_player(p->player);
	p->ep_sq = ep_sq;

	// set current values
	p->hash_key ^= zkeys.ep[p->ep_sq];
	p->hash_key ^= zkeys.ptm[p->player];
}
