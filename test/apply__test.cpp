/*
 * apply__test.cpp
 *
 *  Created on: Nov 27, 2011
 *      Author: james
 */

#include <cassert>
#include <cstring>
#include "../src/defs.h"
#include "../src/globals.h"

void test_apply_move() {
	position pos1;
	reset_pos(&pos1);

	move mv = to_move(PAWN,E2,E4);
	apply_move(&pos1,mv,gundos);

	position pos2;
	set_pos(&pos2,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
	assert(equal_pos(&pos1,&pos2,true));

	////
	mv = to_move(PAWN,C7,C5);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
	assert(equal_pos(&pos1,&pos2,true));

	////
	mv = to_move(KNIGHT,G1,F3);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
	assert(equal_pos(&pos1,&pos2,true));
	assert(pos2.fifty_counter==1);

	////
	mv = to_move(QUEEN,D8,A5);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/q1p5/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3");
	assert(equal_pos(&pos1,&pos2,true));
	assert(pos2.fifty_counter==2);

	/////
	mv = to_move(BISHOP,F1,E2);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/q1p5/4P3/5N2/PPPPBPPP/RNBQK2R b KQkq - 3 3");
	assert(equal_pos(&pos1,&pos2,true));

	////
	mv = to_capture(QUEEN,A5,D2,PAWN);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/2p5/4P3/5N2/PPPqBPPP/RNBQK2R w KQkq - 0 4");
	assert(equal_pos(&pos1,&pos2,true));

	////
	mv = to_capture(KNIGHT,B1,D2,QUEEN);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/2p5/4P3/5N2/PPPNBPPP/R1BQK2R b KQkq - 0 4");
	assert(equal_pos(&pos1,&pos2,true));

	///
	mv = to_move(PAWN,C5,C4);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/8/2p1P3/5N2/PPPNBPPP/R1BQK2R w KQkq - 0 5");
	assert(equal_pos(&pos1,&pos2,true));

	///
	mv = to_move(KING,E1,G1);
	set_castle(&mv);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/8/2p1P3/5N2/PPPNBPPP/R1BQ1RK1 b kq - 0 5");
	assert(equal_pos(&pos1,&pos2,true));


	///
	mv = to_move(KING,E8,D8);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/2p1P3/5N2/PPPNBPPP/R1BQ1RK1 w - - 1 6");
	assert(equal_pos(&pos1,&pos2,true));

	///
	mv = to_move(PAWN,B2,B4);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/1Pp1P3/5N2/P1PNBPPP/R1BQ1RK1 b - b3 0 6");
	assert(equal_pos(&pos1,&pos2,true));

	///
	mv = to_move(PAWN,C4,B3);
	set_epcapture(&mv);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/1p3N2/P1PNBPPP/R1BQ1RK1 w - - 0 7");
	assert(equal_pos(&pos1,&pos2,true));

	////
	mv = to_move(ROOK,F1,E1);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/1p3N2/P1PNBPPP/R1BQR1K1 b - - 1 7");
	assert(equal_pos(&pos1,&pos2,true));

	////
	mv = to_move(PAWN,B3,B2);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/5N2/PpPNBPPP/R1BQR1K1 w - - 0 8");
	assert(equal_pos(&pos1,&pos2,true));

	////
	mv = to_move(KING,G1,H1);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/5N2/PpPNBPPP/R1BQR2K b - - 1 8");
	assert(equal_pos(&pos1,&pos2,true));

	///
	mv = to_capture(PAWN,B2,A1,ROOK);
	set_promopiece(&mv,KNIGHT);
	apply_move(&pos1,mv,gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/5N2/P1PNBPPP/n1BQR2K w - - 0 9");
	assert(equal_pos(&pos1,&pos2,true));

}

void test_undo_double_pawn_push() {
	position pos;
	reset_pos(&pos);

	position save_pos;
	memcpy(&save_pos,&pos,sizeof(position));

	move mv = to_move(PAWN,E2,E4);
	apply_move(&pos,mv,gundos);

	undo_last_move(&pos,gundos);
	assert(equal_pos(&pos,&save_pos,true));
}

void test_undo_capturing_promotion() {
	position pos;
	set_pos(&pos,"r7/1PK5/8/8/k7/8/8/8 w - -");

	position save_pos;
	memcpy(&save_pos,&pos,sizeof(position));

	move mv = to_capture(PAWN,B7,A8,ROOK);
	set_promopiece(&mv,QUEEN);
	apply_move(&pos,mv,gundos);

	undo_last_move(&pos,gundos);
	assert(equal_pos(&pos,&save_pos,true));
}

void test_undo_epcapture() {
	position pos;
	set_pos(&pos,"k7/8/8/8/pP6/8/K7/8 b - b3");

	position save_pos;
	memcpy(&save_pos,&pos,sizeof(position));

	move mv = to_capture(PAWN,A4,B3,PAWN);
	set_epcapture(&mv);
	apply_move(&pos,mv,gundos);

	undo_last_move(&pos,gundos);
	assert(equal_pos(&pos,&save_pos,true));
}

void test_undo_castle() {
	position pos;
	set_pos(&pos,"k7/8/8/8/8/8/8/4K2R w K -");

	position save_pos;
	memcpy(&save_pos,&pos,sizeof(position));

	move mv = to_move(KING,E1,G1);
	set_castle(&mv);
	apply_move(&pos,mv,gundos);

	undo_last_move(&pos,gundos);
	assert(equal_pos(&pos,&save_pos,true));
}

void test_apply() {
	test_apply_move();
	test_undo_double_pawn_push();
	test_undo_capturing_promotion();
	test_undo_epcapture();
	test_undo_castle();
}
