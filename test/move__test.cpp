/*
 * move__test.cpp
 *
 *  Created on: Dec 20, 2011
 *      Author: james
 */

#include <cassert>
#include <cstring>
#include "../src/defs.h"
#include "../src/globals.h"

void test_to_move() {
	move mv = to_move(PAWN,E2,E4);
	assert(get_piece(mv)==PAWN);
	assert(get_from_sq(mv)==E2);
	assert(get_to_sq(mv)==E4);
	assert(!is_epcapture(mv));
	assert(!is_capture(mv));
	assert(!is_castle(mv));
	assert(get_promopiece(mv)==NO_PIECE);
}

void test_set_promopiece() {
	move mv = to_move(PAWN,B2,B1);
	assert(get_piece(mv)==PAWN);
	assert(get_from_sq(mv)==B2);
	assert(get_to_sq(mv)==B1);
	assert(!is_epcapture(mv));
	assert(!is_capture(mv));
	assert(!is_castle(mv));
	assert(get_promopiece(mv)==NO_PIECE);

	set_promopiece(&mv,ROOK);
	assert(get_piece(mv)==PAWN);
	assert(get_from_sq(mv)==B2);
	assert(get_to_sq(mv)==B1);
	assert(!is_epcapture(mv));
	assert(!is_capture(mv));
	assert(!is_castle(mv));
	assert(get_promopiece(mv)==ROOK);

	set_promopiece(&mv,BISHOP);
	assert(get_promopiece(mv)==BISHOP);

	set_promopiece(&mv,QUEEN);
	assert(get_promopiece(mv)==QUEEN);

	set_promopiece(&mv,KNIGHT);
	assert(get_promopiece(mv)==KNIGHT);

	set_promopiece(&mv,NO_PIECE);
	assert(get_piece(mv)==PAWN);
	assert(get_from_sq(mv)==B2);
	assert(get_to_sq(mv)==B1);
	assert(!is_epcapture(mv));
	assert(!is_capture(mv));
	assert(!is_castle(mv));
	assert(get_promopiece(mv)==NO_PIECE);

	///////
	mv = to_capture(PAWN,B2,A1,ROOK);
	set_promopiece(&mv,QUEEN);
	assert(get_piece(mv)==PAWN);
	assert(get_from_sq(mv)==B2);
	assert(get_to_sq(mv)==A1);
	assert(!is_epcapture(mv));
	assert(is_capture(mv));
	assert(!is_castle(mv));
	assert(get_promopiece(mv)==QUEEN);
}

void test_set_capture() {
	move mv = to_move(BISHOP,F3,C6);
	assert(get_piece(mv)==BISHOP);
	assert(get_from_sq(mv)==F3);
	assert(get_to_sq(mv)==C6);
	assert(!is_epcapture(mv));
	assert(!is_capture(mv));
	assert(get_captured_piece(mv)==NO_PIECE);
	assert(!is_castle(mv));
	assert(get_promopiece(mv)==NO_PIECE);

	set_capture(&mv,PAWN);
	assert(get_piece(mv)==BISHOP);
	assert(get_from_sq(mv)==F3);
	assert(get_to_sq(mv)==C6);
	assert(!is_epcapture(mv));
	assert(is_capture(mv));
	assert(get_captured_piece(mv)==PAWN);
	assert(!is_castle(mv));
	assert(get_promopiece(mv)==NO_PIECE);
}

void test_set_epcapture() {
	move mv = to_move(PAWN,D4,C3);
	set_epcapture(&mv);
	assert(get_piece(mv)==PAWN);
	assert(get_from_sq(mv)==D4);
	assert(get_to_sq(mv)==C3);
	assert(is_epcapture(mv));
	assert(is_capture(mv));
	assert(get_captured_piece(mv)==PAWN);
	assert(!is_castle(mv));
	assert(get_promopiece(mv)==NO_PIECE);
}

void test_set_castle() {
	move mv = to_move(KING,E1,H1);
	set_castle(&mv);
	assert(get_piece(mv)==KING);
	assert(get_from_sq(mv)==E1);
	assert(get_to_sq(mv)==H1);
	assert(!is_epcapture(mv));
	assert(!is_capture(mv));
	assert(is_castle(mv));
	assert(get_promopiece(mv)==NO_PIECE);
}

void test_move_score() {
	move mv = to_move(PAWN,E2,E4);
	assert(get_move_score(&mv)==0);

	set_move_score(&mv,100);
	assert(get_move_score(&mv)==100);

	set_move_score(&mv,-9);
	assert(get_move_score(&mv)==-9);

	set_move_score(&mv,CHECKMATE);
	assert(get_move_score(&mv)==CHECKMATE);

	set_move_score(&mv,-CHECKMATE);
	assert(get_move_score(&mv)==-CHECKMATE);

	set_move_score(&mv,-CHECKMATE+10);
	assert(get_move_score(&mv)==-CHECKMATE+10);

	// make sure we haven't corrupted the move itself
	assert(get_piece(mv)==PAWN);
	assert(get_from_sq(mv)==E2);
	assert(get_to_sq(mv)==E4);
}

void test_move() {
	test_to_move();
	test_set_promopiece();
	test_set_capture();
	test_set_epcapture();
	test_set_castle();
	test_move_score();
}

