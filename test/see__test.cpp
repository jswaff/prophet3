/*
 * see_test.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: James
 */


#include <cassert>
#include <cstdlib>
#include <cstring>
#include "../src/globals.h"
#include "../src/defs.h"
#include "../src/eval.h"

void test_queen_takes_undefended_pawn() {
	position pos;
	set_pos(&pos,"7k/8/1p6/8/8/1Q6/8/7K w - -");

	move mv = to_capture(QUEEN,B3,B6,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val);
}

void test_queen_takes_defended_pawn() {
	position pos;
	set_pos(&pos,"7k/p7/1p6/8/8/1Q6/8/7K w - -");

	move mv = to_capture(QUEEN,B3,B6,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val-queen_val);
}

void test_rook_takes_undefended_pawn() {
	position pos;
	set_pos(&pos,"1k1r4/1pp4p/p7/4p3/8/P5P1/1PP4P/2K1R3 w - -");

	move mv = to_capture(ROOK,E1,E5,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val);
}

void test_with_xrays() {
	position pos;
	set_pos(&pos,"1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - -");

	move mv = to_capture(KNIGHT,D3,E5,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val-knight_val);
}

void test_rook_xrays() {
	position pos;
	set_pos(&pos,"3kr3/8/4p3/8/8/4R3/4R3/4K3 w - -");

	move mv = to_capture(ROOK,E3,E6,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val);
}

void test_knight_takes_defended_pawn_as_white() {
	position pos;
	set_pos(&pos,"k7/8/5n2/3p4/8/2N2B2/8/K7 w - -");

	move mv = to_capture(KNIGHT,C3,D5,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val);
}

void test_knight_takes_defended_pawn_as_black() {
	position pos;
	set_pos(&pos,"K7/8/5N2/3P4/8/2n2b2/8/k7 b - -");

	move mv = to_capture(KNIGHT,C3,D5,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val);
}

void test_crazy_rooks() {
	position pos;
	set_pos(&pos,"2K5/8/8/3pRrRr/8/8/8/2k5 w - -");

	move mv = to_capture(ROOK,E5,D5,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val-rook_val);
}

void test_crazy_rooks2() {
	position pos;
	set_pos(&pos,"2K5/8/8/3pRrR1/8/8/8/2k5 w - -");
	move mv = to_capture(ROOK,E5,D5,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val);
}

void test_knight_takes_defended_pawn() {
	position pos;
	set_pos(&pos,"1K1k4/8/5n2/3p4/8/1BN5/8/8 w - -");

	move mv = to_capture(KNIGHT,C3,D5,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val);
}

void test_bishop_takes_defended_pawn() {
	position pos;
	set_pos(&pos,"1K1k4/8/5n2/3p4/8/1BN5/8/8 w - -");

	move mv = to_capture(BISHOP,B3,D5,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val-bishop_val+knight_val);
}

void test_knight_takes_defended_pawn_with_crazy_bishops() {
	position pos;
	set_pos(&pos,"1K1k4/8/5n2/3p4/8/2N2B2/6b1/7b w - -");

	move mv = to_capture(KNIGHT,C3,D5,PAWN);
	assert(is_legal_move(mv,&pos));
	apply_move(&pos,mv,gundos);

	int32 score = see(&pos,mv);
	assert(score==pawn_val-knight_val);
}

void test_case_see(const char* fen,const char* str_mv,int32 score) {
	position pos;
	set_pos(&pos,fen);

	move mv = str_to_move(str_mv,&pos);
	assert(is_legal_move(mv,&pos));
	int32 captured_piece = is_epcapture(mv) ? PAWN : (piece_t)pos.piece[get_to_sq(mv)];
	if (captured_piece < NO_PIECE) captured_piece = -captured_piece;
	assert(captured_piece==get_captured_piece(mv));
	apply_move(&pos,mv,gundos);
	int32 my_score = see(&pos,mv);
	assert(my_score==score);
}

void test_see() {
	test_queen_takes_undefended_pawn();
	test_queen_takes_defended_pawn();
	test_rook_takes_undefended_pawn();
	test_with_xrays();
	test_rook_xrays();
	test_knight_takes_defended_pawn_as_white();
	test_knight_takes_defended_pawn_as_black();
	test_crazy_rooks();
	test_crazy_rooks2();
	test_knight_takes_defended_pawn();
	test_bishop_takes_defended_pawn();
	test_knight_takes_defended_pawn_with_crazy_bishops();

	test_case_see("4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - -","hxg4",0);
	test_case_see("4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - -","hxg4",0);
	test_case_see("4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - -","Bxf3",knight_val-bishop_val);
	test_case_see("2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - -","dxe5",pawn_val);
	test_case_see("7r/5qpk/p1Qp1b1p/3r3n/BB3p2/5p2/P1P2P2/4RK1R w - -","Re8",0);
	test_case_see("7R/4bP2/8/8/1q6/3K4/5p2/4k3 w - -","f8=R",10004);
	test_case_see("8/4kp2/2npp3/1Nn5/1p2PQP1/7q/1PP1B3/4KR1r b - -","Rxf1+",0);
	test_case_see("2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - -","Qxc1",2*rook_val-queen_val);
	test_case_see("r2qk1nr/pp2ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq -","Nxe5",pawn_val);
	test_case_see("6r1/4kq2/b2p1p2/p1pPb3/p1P2B1Q/2P4P/2B1R1P1/6K1 w - -","Bxe5",0);
	test_case_see("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6","gxh6",0);
	test_case_see("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R1B2B/PQ3P1P/3R2K1 w - h6","gxh6",pawn_val);
	test_case_see("2r4r/1P4pk/p2p1b1p/7n/BB3p2/2R2p2/P1P2P2/4RK2 w - -","Rxc8",rook_val);

	// Arasan says +rook for this, but RxR then BxR then PxB ==> Bishop Val
	// Note the last pawn capture is also a promotion so that could be added
	test_case_see("2r5/1P4pk/p2p1b1p/5b1n/BB3p2/2R2p2/P1P2P2/4RK2 w - -","Rxc8",bishop_val);

	test_case_see("2r4k/2r4p/p7/2b2p1b/4pP2/1BR5/P1R3PP/2Q4K w - -","Rxc5",bishop_val);
	test_case_see("8/pp6/2pkp3/4bp2/2R3b1/2P5/PP4B1/1K6 w - -","Bxc6",pawn_val-bishop_val);
	test_case_see("4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -","Rxe4",pawn_val-rook_val);
	test_case_see("4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -","Bxe4",pawn_val);

/*
	// not captures
	//test_case_see("6rr/6pk/p1Qp1b1p/2n5/1B3p2/5p2/P1P2P2/4RK1R w - -","Re8",-Eval.ROOK_VAL);
	//test_case_see("7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - -","Re8",-Eval.ROOK_VAL);

	// promotion that doesn't capture
	//test_case_see("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -","f8=Q",Eval.BISHOP_VAL-Eval.PAWN_VAL);
	//test_case_see("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -","f8=N",Eval.KNIGHT_VAL-Eval.PAWN_VAL);
	//test_case_see("7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -","f8=Q",Eval.QUEEN_VAL-Eval.PAWN_VAL);
	//test_case_see("7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -","f8=B",Eval.BISHOP_VAL-Eval.PAWN_VAL);
*/
}
