/*
 * defs__test.cpp
 *
 *  Created on: Nov 20, 2011
 *      Author: james
 */


#include <cassert>
#include <cmath>
#include "../src/defs.h"
#include "../src/globals.h"
#include "../src/eval.h"

void test_squares() {
	assert(A8==0);
	assert(H8==7);
	assert(A1==56);
	assert(H1==63);
	assert(NO_SQUARE==64);
}

void test_pieces() {
	assert(NO_PIECE==0);
	assert(NO_PIECE < PAWN);
	assert(PAWN < BISHOP);
	assert(PAWN < KNIGHT);
	assert(BISHOP < ROOK);
	assert(KNIGHT < ROOK);
	assert(ROOK < QUEEN);
	assert(QUEEN < KING);
}

void test_fliprank() {
	for (int sq=0;sq<64;sq++) {
		assert(sq==fliprank[fliprank[sq]]);
	}
}

void test_defs() {
	test_squares();
	test_pieces();
	test_fliprank();
}

