/*
 * evalp_test.cpp
 *
 *  Created on: Jan 23, 2016
 *      Author: James
 */

#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"
#include "../src/eval.h"

void test_passed_pawn_initial_pos() {
	position pos;
	reset_pos(&pos);

	for (int i=0;i<64;i++) {
		if (pos.piece[i]==PAWN || pos.piece[i]==-PAWN) {
			assert(!is_passed(&pos,(square_t)i,pos.piece[i]==PAWN));
			assert(!is_doubled(&pos,(square_t)i,pos.piece[i]==PAWN));
			assert(!is_isolated(&pos,(square_t)i,pos.piece[i]==PAWN));
		}
	}
}

void test_passed_pawn_wiki_pos() {
	position pos;
	set_pos(&pos,"7k/8/7p/1P2Pp1P/2Pp1PP1/8/8/7K w - -");

	assert(is_passed(&pos,B5,true));
	assert(is_passed(&pos,C4,true));
	assert(is_passed(&pos,D4,false));
	assert(is_passed(&pos,E5,true));

	assert(!is_passed(&pos,F5,false));
	assert(!is_passed(&pos,F4,true));
	assert(!is_passed(&pos,G4,true));
	assert(!is_passed(&pos,H5,true));
	assert(!is_passed(&pos,H6,false));
}

void test_passed_pawn_wiki_pos2() {
	position pos;
	set_pos(&pos,"8/5ppp/8/5PPP/8/6k1/8/6K1 w - -");

	assert(!is_passed(&pos,F7,false));
	assert(!is_passed(&pos,G7,false));
	assert(!is_passed(&pos,H7,false));
	assert(!is_passed(&pos,F5,true));
	assert(!is_passed(&pos,G5,true));
	assert(!is_passed(&pos,H5,true));
}

void test_passed_pawn_wiki_pos3() {
	position pos;
	set_pos(&pos,"8/8/1PP2PbP/3r4/8/1Q5p/p5N1/k3K3 b - -");

	assert(is_passed(&pos,B6,true));
	assert(is_passed(&pos,C6,true));
	assert(is_passed(&pos,F6,true));
	assert(is_passed(&pos,H6,true));
	assert(is_passed(&pos,A2,false));
	assert(is_passed(&pos,H3,false));
}

void test_passed_pawn_wiki_pos4() {
	position pos;
	set_pos(&pos,"k7/b1P5/KP6/6q1/8/8/8/4n3 b - -");

	assert(is_passed(&pos,B6,true));
	assert(is_passed(&pos,C7,true));
}

void test_levinfish_smyslov57() {
	position pos;
	set_pos(&pos,"R7/6k1/P5p1/5p1p/5P1P/r5P1/5K2/8 w - -");

	assert(is_passed(&pos,A6,true));
	assert(!is_passed(&pos,G6,false));
	assert(!is_passed(&pos,F5,false));
	assert(!is_passed(&pos,H5,false));
	assert(!is_passed(&pos,F4,true));
	assert(!is_passed(&pos,H4,true));
	assert(!is_passed(&pos,G3,true));
}

void test_fischer_larsen71() {
	position pos;
	set_pos(&pos,"8/4kp2/6p1/7p/P7/2K3P1/7P/8 b - -");

	assert(!is_passed(&pos,F7,false));
	assert(!is_passed(&pos,G6,false));
	assert(!is_passed(&pos,H5,false));
	assert(is_passed(&pos,A4,true));
	assert(!is_passed(&pos,G3,true));
	assert(!is_passed(&pos,H2,true));
}

void test_botvinnik_capablanca38() {
	position pos;
	set_pos(&pos,"8/p3q1kp/1p2Pnp1/3pQ3/2pP4/1nP3N1/1B4PP/6K1 w - -");

	assert(is_passed(&pos,A7,false));
	assert(!is_passed(&pos,H7,false));
	assert(!is_passed(&pos,B6,false));
	assert(!is_passed(&pos,G6,false));
	assert(!is_passed(&pos,D5,false));
	assert(!is_passed(&pos,C4,false));
	assert(!is_passed(&pos,D4,true));
	assert(!is_passed(&pos,C3,true));
	assert(!is_passed(&pos,G2,true));
	assert(!is_passed(&pos,H2,true));
}

void test_isolated_pawn() {
	position pos;
	set_pos(&pos,"k7/p1p3p1/3p3p/1P5P/1PP1P1P1/8/8/K7 w - - 0 ");

	// white's pawn on the E file and black's pawn on the A file are isolated
	assert(!is_passed(&pos,E4,true));
	assert(!is_doubled(&pos,E4,true));
	assert(is_isolated(&pos,E4,true));

	assert(!is_passed(&pos,A7,false));
	assert(!is_doubled(&pos,A7,false));
	assert(is_isolated(&pos,A7,false));

	assert(!is_passed(&pos,C7,false));
	assert(!is_doubled(&pos,C7,false));
	assert(!is_isolated(&pos,C7,false));

	assert(!is_passed(&pos,G4,WHITE));
	assert(!is_doubled(&pos,G4,WHITE));
	assert(!is_isolated(&pos,G4,WHITE));
}

void test_doubled() {
	position pos;
	set_pos(&pos,"k7/p1p3p1/3p3p/1P5P/1PP1P1P1/8/8/K7 w - - 0 1");

	assert(!is_passed(&pos,B5,true));
	assert(is_doubled(&pos,B5,true));
	assert(!is_isolated(&pos,B5,true));

	assert(!is_passed(&pos,B4,true));
	assert(is_doubled(&pos,B4,true));
	assert(!is_isolated(&pos,B4,true));
}

void test_evalp() {
	test_passed_pawn_initial_pos();
	test_passed_pawn_wiki_pos();
	test_passed_pawn_wiki_pos2();
	test_passed_pawn_wiki_pos3();
	test_passed_pawn_wiki_pos4();
	test_levinfish_smyslov57();
	test_fischer_larsen71();
	test_botvinnik_capablanca38();
	test_isolated_pawn();
	test_doubled();
}
