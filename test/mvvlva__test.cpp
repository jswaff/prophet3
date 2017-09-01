/*
 * score__test.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: James
 */


#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"

move get_highest_scoring(position *pos,move *startp,move *endp) {
	move* bestp = 0;
	int32 bestscore = 0;

	for (move *mp=startp;mp<endp;mp++) {
		if (*mp != 0) {
			int32 score = mvvlva(pos,*mp);
			if (bestp==0 || score > bestscore) {
				bestp = mp;
				bestscore = score;
			}
		}
	}

	move bestmv = *bestp;
	*bestp = 0;
	return bestmv;
}

void test_mvvlva1() {
	position pos;
	set_pos(&pos,"3r1rk1/p3qp1p/2bb2p1/2p5/3P4/1P6/PBQN1PPP/2R2RK1 b - - ");
	move moves[50],*endp;
	endp = gen_legal_moves(moves,&pos,true,true); // 46 moves

	move c5d4 = to_capture(PAWN,C5,D4,PAWN);
	move c6g2 = to_capture(BISHOP,C6,G2,PAWN);
	move d6h2 = to_capture(BISHOP,D6,H2,PAWN);

	move bestmv = get_highest_scoring(&pos,moves,endp);
	move bestmv2 = get_highest_scoring(&pos,moves,endp);
	move bestmv3 = get_highest_scoring(&pos,moves,endp);

	assert(bestmv==c5d4);
	assert(bestmv2 != bestmv3);
	assert(bestmv2==c6g2 || bestmv2==d6h2);
	assert(bestmv3==c6g2 || bestmv3==d6h2);
}

void test_mvvlva2() {
	position pos;
	set_pos(&pos,"8/4Pk1p/6p1/1r6/8/5N2/2B2PPP/b5K1 w - - ");

	move moves[50],*endp;
	endp = gen_legal_moves(moves,&pos,true,true); // 24 moves

	move e7e8q = to_move(PAWN,E7,E8);
	set_promopiece(&e7e8q,QUEEN);

	move e7e8r = to_move(PAWN,E7,E8);
	set_promopiece(&e7e8r,ROOK);

	move e7e8b = to_move(PAWN,E7,E8);
	set_promopiece(&e7e8b,BISHOP);

	move e7e8n = to_move(PAWN,E7,E8);
	set_promopiece(&e7e8n,KNIGHT);

	move c2g6 = to_capture(BISHOP,C2,G6,PAWN);

	assert(is_in_move_list(e7e8q,moves,endp));
	assert(is_in_move_list(e7e8r,moves,endp));
	assert(is_in_move_list(e7e8b,moves,endp));
	assert(is_in_move_list(e7e8n,moves,endp));
	assert(is_in_move_list(c2g6,moves,endp));

	assert(get_highest_scoring(&pos,moves,endp)==e7e8q);
	assert(get_highest_scoring(&pos,moves,endp)==e7e8r);
	assert(get_highest_scoring(&pos,moves,endp)==e7e8b);
	assert(get_highest_scoring(&pos,moves,endp)==e7e8n);
	assert(get_highest_scoring(&pos,moves,endp)==c2g6);
}

void test_mvvlva3() {
	position pos;
	set_pos(&pos,"6r1/pp1b1P1p/5Q2/3p3k/5K2/8/2P3P1/8 w - - ");

	move moves[50],*endp;
	endp = gen_legal_moves(moves,&pos,true,true); // 39 moves

	move f7f8q = to_move(PAWN,F7,F8);
	set_promopiece(&f7f8q,QUEEN);

	move f7f8r = to_move(PAWN,F7,F8);
	set_promopiece(&f7f8r,ROOK);

	move f7f8b = to_move(PAWN,F7,F8);
	set_promopiece(&f7f8b,BISHOP);

	move f7f8n = to_move(PAWN,F7,F8);
	set_promopiece(&f7f8n,KNIGHT);

	move f7g8q = to_capture(PAWN,F7,G8,ROOK);
	set_promopiece(&f7g8q,QUEEN);

	move f7g8r = to_capture(PAWN,F7,G8,ROOK);
	set_promopiece(&f7g8r,ROOK);

	move f7g8b = to_capture(PAWN,F7,G8,ROOK);
	set_promopiece(&f7g8b,BISHOP);

	move f7g8n = to_capture(PAWN,F7,G8,ROOK);
	set_promopiece(&f7g8n,KNIGHT);

	assert(is_in_move_list(f7f8q,moves,endp));
	assert(is_in_move_list(f7f8r,moves,endp));
	assert(is_in_move_list(f7f8b,moves,endp));
	assert(is_in_move_list(f7f8n,moves,endp));
	assert(is_in_move_list(f7g8q,moves,endp));
	assert(is_in_move_list(f7g8r,moves,endp));
	assert(is_in_move_list(f7g8b,moves,endp));
	assert(is_in_move_list(f7g8n,moves,endp));

	assert(get_highest_scoring(&pos,moves,endp)==f7g8q);
	assert(get_highest_scoring(&pos,moves,endp)==f7g8r);
	assert(get_highest_scoring(&pos,moves,endp)==f7g8b);
	assert(get_highest_scoring(&pos,moves,endp)==f7g8n);
	assert(get_highest_scoring(&pos,moves,endp)==f7f8q);
	assert(get_highest_scoring(&pos,moves,endp)==f7f8r);
	assert(get_highest_scoring(&pos,moves,endp)==f7f8b);
	assert(get_highest_scoring(&pos,moves,endp)==f7f8n);
}

void test_mvvlva4() {
	position pos;
	set_pos(&pos,"6R1/kp6/8/1KpP4/8/8/8/6B1 w - c6");

	move moves[50],*endp;
	endp = gen_legal_moves(moves,&pos,true,true); // 28 moves

	move d5c6 = to_capture(PAWN,D5,C6,PAWN);
	set_epcapture(&d5c6);

	move b5c5 = to_capture(KING,B5,C5,PAWN);
	move g1c5 = to_capture(BISHOP,G1,C5,PAWN);

	assert(is_in_move_list(d5c6,moves,endp));
	assert(is_in_move_list(b5c5,moves,endp));
	assert(is_in_move_list(g1c5,moves,endp));

	assert(get_highest_scoring(&pos,moves,endp)==d5c6);
	assert(get_highest_scoring(&pos,moves,endp)==g1c5);
	assert(get_highest_scoring(&pos,moves,endp)==b5c5);
}

void test_mvvlva() {
	test_mvvlva1();
	test_mvvlva2();
	test_mvvlva3();
	test_mvvlva4();
}
