/*
 * eval__test.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: james
 */

#include <cassert>
#include <cstring>
#include "../src/defs.h"
#include "../src/globals.h"
#include "../src/eval.h"

void test_start_pos_is_0() {
	position pos;
	reset_pos(&pos);
	search_stats stats;
	int32 score=eval(&pos,false,&stats);
	assert(score==0);
}

void test_eval_pawn() {
	position pos;
	reset_pos(&pos);
	assert(eval_pawn(&pos,E2,true)==pawn_pst[E2]);
	assert(eval_pawn(&pos,B7,false)==pawn_pst[B2]);
	assert(eval_pawn(&pos,B7,false)==eval_pawn(&pos,B2,true));
}

void test_eval_knight() {
	position pos;
	reset_pos(&pos);
	assert(eval_knight(&pos,F3,true)==knight_pst[F3] + knight_tropism * distance(F3,E8));
	assert(eval_knight(&pos,D6,true)==knight_pst[D6] + knight_tropism * distance(D6,E8));
	assert(eval_knight(&pos,H3,false)==knight_pst[H6] + knight_tropism * distance(H6,E8));
	assert(eval_knight(&pos,F7,false)==knight_pst[F2] + knight_tropism * distance(F2,E8));
}

void test_eval_bishop() {
	position pos;
	search_stats stats;

	set_pos(&pos,"6k1/3B4/8/8/8/8/8/K7 w - - 0 1");
	assert(eval(&pos,false,&stats)==bishop_val + bishop_pst[D7]);

	set_pos(&pos,"6k1/8/8/3B4/8/8/8/K7 w - - 0 1");
	assert(eval(&pos,false,&stats)==bishop_val + bishop_pst[D5]);
}

void test_eval_rook() {
	position pos;
	search_stats stats;

	set_pos(&pos,"6k1/3R4/8/8/8/8/8/3K4 w - - 0 1");
	int32 score = eval(&pos,false,&stats);
	assert(score==rook_val + rook_on_7th + rook_open_file);

	set_pos(&pos,"6k1/3RR3/8/8/8/8/8/3K4 w - - 0 1");
	score = eval(&pos,false,&stats);
	assert(score==rook_val*2 + rook_on_7th*2 + connected_majors_on_7th + rook_open_file*2);

	set_pos(&pos,"6k1/3RRr2/8/8/8/8/8/3K4 w - - 0 1");
	score = eval(&pos,false,&stats);
	// white has two rooks on open files and black one, so net 1 for white
	assert(score==rook_val + rook_on_7th*2 + connected_majors_on_7th + rook_open_file);

	set_pos(&pos,"6k1/3RRr2/8/8/8/8/r7/3K4 w - - 0 1");
	score = eval(&pos,false,&stats);
	assert(score==rook_on_7th + connected_majors_on_7th);

	set_pos(&pos,"6k1/3RRr2/8/8/8/r7/8/3K4 w - - 0 1");
	score = eval(&pos,false,&stats);
	// deduct for black for rook on A file
	assert(score==rook_on_7th*2 + connected_majors_on_7th - rook_pst[fliprank[A3]]);

	set_pos(&pos,"6k1/8/8/8/8/8/qr6/7K b - - 0 1");
	score = eval(&pos,false,&stats);
	assert(score==queen_val + rook_val + rook_on_7th*2 + connected_majors_on_7th
			+ rook_open_file
			+ queen_pst[A2]
			- eval_scale(king_safety_pawn_far_away*2+king_safety_pawn_far_away/2,queen_val+rook_val));
}

void test_eval_rook_on_7th() {
	position pos;
	search_stats stats;
	set_pos(&pos,"7k/8/8/8/8/8/r7/7K w - - 0 1");
	int32_pair npmat = eval_nonpawn_material(&pos);
	assert(npmat.val1==0);
	assert(npmat.val2==rook_val);

	assert(eval(&pos,false,&stats)==-rook_val - rook_on_7th - rook_open_file);

	// move king out
	set_pos(&pos,"7k/8/8/8/8/7K/r7/8 w - - 0 1");
	assert(eval(&pos,false,&stats)==-rook_val - rook_open_file);

	// flipped
	set_pos(&pos,"7k/8/8/8/8/7K/r7/8 b - - 0 1");
	assert(eval(&pos,false,&stats)==rook_val  + rook_open_file);
}

void test_eval_connected_majors_7th() {
	position pos;
	search_stats stats;
	set_pos(&pos,"7k/R2R4/8/8/8/8/8/7K w - - 0 1");
	assert(eval(&pos,false,&stats)==rook_val*2 + rook_on_7th*2 + connected_majors_on_7th
			+ rook_open_file*2);

	set_pos(&pos,"7k/R2RQ3/8/8/8/8/8/7K w - - 0 1");
	int32 expected = rook_val*2 + queen_val
			+ rook_on_7th*3
			+ connected_majors_on_7th*2
			+ rook_open_file*2
			- eval_scale(king_safety_pawn_far_away // f file
			           + king_safety_pawn_far_away // g file
			           + (king_safety_pawn_far_away/2), // h file
			           rook_val*2+queen_val);
	assert(eval(&pos,false,&stats)==expected);

	// flip it
	set_pos(&pos,"7k/R2QR3/8/8/8/8/8/7K b - - 0 1");
	assert(eval(&pos,false,&stats)==-expected);
}

void test_eval_rook_open_file() {
	position pos;
	search_stats stats;

	set_pos(&pos,"3r3k/8/8/8/8/8/8/7K b - - 0 1");
	assert(eval(&pos,false,&stats)==rook_val + rook_open_file);

	set_pos(&pos,"3r3k/3r4/8/8/8/8/8/7K b - - 0 1");
	assert(eval(&pos,false,&stats)==rook_val*2 + rook_open_file*2);

	set_pos(&pos,"3r3k/8/3p4/8/8/8/8/7K b - - 0 1");
	assert(eval(&pos,false,&stats)==rook_val+pawn_val +passed_pawn+isolated_pawn
			+ pawn_pst[fliprank[D6]]);
}

void test_eval_rook_half_open_file() {
	position pos;
	search_stats stats;

	set_pos(&pos,"8/2P5/8/2R5/K7/8/7k/8 w - - 0 1");
	assert(eval(&pos,false,&stats)==rook_val + pawn_val  + passed_pawn + isolated_pawn
			+ pawn_pst[C7]);

	set_pos(&pos,"8/2p5/8/2R5/K7/8/7k/8 w - - 0 1");
	assert(eval(&pos,false,&stats)==rook_val-pawn_val -passed_pawn-isolated_pawn
			+ pawn_pst[fliprank[C7]]
			+ rook_half_open_file);
}

void test_eval_queen() {
	position pos;
	search_stats stats;

	set_pos(&pos,"3kq3/8/8/8/8/8/8/3K4 b - - 0 1");
	assert(eval(&pos,false,&stats)==queen_val + queen_pst[fliprank[E8]]);
	int32_pair npmat = eval_nonpawn_material(&pos);
	assert(npmat.val1==0);
	assert(npmat.val2==queen_val);
}

void test_king_pst() {
	position pos;
	search_stats stats;

	set_pos(&pos,"rnbqkbnr/pppppppp/8/8/3P4/3BPN2/PPP2PPP/RNBQR1K1 w - - 0 1");
	int32_pair npmat = eval_nonpawn_material(&pos);
	assert(0==npmat.val1-npmat.val2);

	int32_pair pmat = eval_pawn_material(&pos);
	assert(0==pmat.val1-pmat.val2);

	int32 score = eval(&pos,false,&stats);

	int32 tropism_delta = knight_tropism * distance(F3,pos.black_king)
		- knight_tropism * distance(G1,pos.black_king);

	assert(score==pawn_pst[D4] -pawn_pst[D2] + pawn_pst[E3] -pawn_pst[E2]
	             + bishop_pst[D3] - bishop_pst[F1]
	             + knight_pst[F3] - knight_pst[G1]
	             + rook_pst[E1] - rook_pst[H1]
	             + king_pst[G1] - king_pst[E1]
	             + tropism_delta);

}

void test_king_endgame_pst() {
	position pos;
	search_stats stats;

	set_pos(&pos,"8/8/8/4k3/8/8/8/RNB2K2 w - - 0 1");

	int32_pair npmat = eval_nonpawn_material(&pos);
	int32 expected = rook_val + knight_val + bishop_val;
	assert(expected==npmat.val1);
	assert(0==npmat.val2);

	expected += rook_open_file + knight_pst[B1]
	        + knight_tropism * distance(B1,pos.black_king)
	        + king_endgame_pst[F1] - king_pst[E5]
	        - eval_scale(king_safety_middle_open_file,rook_val+knight_val+bishop_val);
	assert(expected==eval(&pos,false,&stats));

	// by removing a bishop both sides should be evaluated as in the endgame
	set_pos(&pos,"8/8/8/4k3/8/8/8/RN3K2 b - - 0 1");

	expected = -rook_val - knight_val - rook_open_file
			- knight_pst[B1]
			- knight_tropism * distance(B1,pos.black_king)
			- king_endgame_pst[F1]
			+ king_endgame_pst[E5];
	assert(expected==eval(&pos,false,&stats));
}

void test_king_safety_middle_files() {
	position pos;
	search_stats stats;

	// initial position then e3... no penalty
	set_pos(&pos,"rnbqkbnr/pppppppp/8/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
	int32 score = eval(&pos,false,&stats);
	assert(score==pawn_pst[E3]-pawn_pst[E2]);

	// open file for both, so still no penalty
	set_pos(&pos,"rnbqkbnr/pppp1ppp/8/8/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
	score = eval(&pos,false,&stats);
	assert(score==0);

	// remove both queens.  open e file.  put black king on d8
	set_pos(&pos,"rnbk1bnr/pppp1ppp/8/8/8/8/PPPP1PPP/RNB1KBNR b KQ - 0 1");
	score = eval(&pos,false,&stats);
	assert(score== -eval_scale(king_safety_middle_open_file,
			rook_val*2 + knight_val*2 + bishop_val*2) + king_pst[fliprank[D8]]);
}

void test_king_safety_kingside() {
	position pos;
	search_stats stats;

	// no pawns advanced
	set_pos(&pos,"rnbq1rk1/pppppppp/8/8/8/8/PPPPPPPP/RNBQ1RK1 w kq - 0 1");
	assert(eval(&pos,false,&stats)==0);

	// white pawn on f4
	set_pos(&pos,"rnbq1rk1/pppppppp/8/8/8/5P2/PPPPP1PP/RNBQ1RK1 w kq - 0 1");
	assert(eval(&pos,false,&stats)==pawn_pst[F3]-pawn_pst[F2]
		+ eval_scale(king_safety_pawn_one_away,rook_val*2+knight_val+bishop_val+queen_val));

	// white pawn on g4
	set_pos(&pos,"rnbq1rk1/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQ1RK1 w kq - 0 1");
	assert(eval(&pos,false,&stats)==pawn_pst[G4]-pawn_pst[G2]+eval_scale(king_safety_pawn_two_away,rook_val*2+knight_val+bishop_val+queen_val));

	// black pawn on h4
	set_pos(&pos,"rnbq1rk1/ppppppp1/8/8/7p/8/PPPPPPPP/RNBQ1RK1 b KQkq - 0 1");
	assert(eval(&pos,false,&stats)==pawn_pst[fliprank[H4]]-pawn_pst[fliprank[H7]]
	     +eval_scale(king_safety_pawn_far_away/2,rook_val*2+knight_val+bishop_val+queen_val));
}

void test_king_safety_queenside() {
	position pos;
	search_stats stats;

	// white pawn on c3
	set_pos(&pos,"1krq1bnr/pppppppp/8/8/8/2P5/PP1PPPPP/1KRQ1BNR w kq - 0 1");
	assert(eval(&pos,false,&stats)==pawn_pst[C3]-pawn_pst[C2]+eval_scale(king_safety_pawn_one_away,rook_val*2+queen_val+bishop_val+knight_val));

	// white pawn on b4
	set_pos(&pos,"1krq1bnr/pppppppp/8/8/1P6/8/P1PPPPPP/1KRQ1BNR w kq - 0 1");
	assert(eval(&pos,false,&stats)==pawn_pst[B4]-pawn_pst[B2]+eval_scale(king_safety_pawn_two_away,rook_val*2+queen_val+bishop_val+knight_val));

	// black pawn on a4
	set_pos(&pos,"1krq1bnr/1ppppppp/8/8/p7/8/PPPPPPPP/1KRQ1BNR b kq - 0 1");
	assert(eval(&pos,false,&stats)==pawn_pst[fliprank[A4]]-pawn_pst[fliprank[A7]]
       +eval_scale(king_safety_pawn_far_away/2,rook_val*2+queen_val+bishop_val+knight_val));
}

void test_double_and_isolated_pawns() {
	position pos;
	search_stats stats;

	set_pos(&pos,"k7/p1p3p1/3p3p/1P5P/1PP1P3/8/8/K7 b - - 0 1");
	int32_pair npmat = eval_nonpawn_material(&pos);
	assert(0==npmat.val1);
	assert(0==npmat.val2);

	int32_pair pmat = eval_pawn_material(&pos);
	assert(pawn_val*5==pmat.val1);
	assert(pawn_val*5==pmat.val2);

	int32 expected = king_endgame_pst[fliprank[A8]]
           + pawn_pst[fliprank[A7]] + pawn_pst[fliprank[C7]] + pawn_pst[fliprank[D6]]
           + pawn_pst[fliprank[G7]] + pawn_pst[fliprank[H6]]
           + isolated_pawn // black pawn on a7
           - king_endgame_pst[A1]
           - pawn_pst[B5] - pawn_pst[B4] - pawn_pst[C4] - pawn_pst[E4] - pawn_pst[H5]
           - isolated_pawn*2 // white pawns on e4 and h1
           - doubled_pawn*2;  // white pawns on b4 and b5
	assert(eval(&pos,false,&stats)==expected);
}

void test_score1() {
	position pos;
	search_stats stats;

	set_pos(&pos,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
	assert(eval(&pos,false,&stats)==-(pawn_pst[E4]-pawn_pst[E2]));
}

void test_score2() {
	position pos;
	search_stats stats;

	set_pos(&pos,"rnbqkbnr/pp1ppppp/2p5/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
	assert(eval(&pos,false,&stats)==pawn_pst[E4]-pawn_pst[E2]-pawn_pst[fliprank[C6]]+pawn_pst[fliprank[C7]]);
}

void testcase_symmetry(position *pos,const char *fen) {
	search_stats stats;
	set_pos(pos,fen);
	int32 score=eval(pos,false,&stats);
	test_eval_symmetry(score,pos);
}

bool test_eval_symmetry(int32 score,position *pos) {
	position flip_pos;
	search_stats stats;

	memcpy(&flip_pos,pos,sizeof(position));
	flip_board(&flip_pos);

	int32 score2=eval(&flip_pos,false,&stats);
	assert(score==score2);

	flip_board(&flip_pos);
	assert(equal_pos(pos,&flip_pos,true));

	return true;
}

void test_vertical_symmetry() {
	position pos;
	testcase_symmetry(&pos,"7r/R6p/2K4P/5k1P/2p4n/5p2/8/8 w - - 0 1");
	testcase_symmetry(&pos,"8/k3Nr2/2rR4/1P1n4/6p1/1K6/8/6n1 w - - 0 1");
	testcase_symmetry(&pos,"8/4K3/8/1NR5/8/4k1r1/8/8 w - -");
	testcase_symmetry(&pos,"8/4K3/8/1N6/6p1/4k2p/8/8 w - -");
	testcase_symmetry(&pos,"8/4K3/8/1r6/6B1/4k2N/8/8 w - -");
	testcase_symmetry(&pos,"3b4/1n3n2/1pk3Np/p7/P4P1p/1P6/5BK1/3R4 b - -");
	testcase_symmetry(&pos,"8/3r1ppk/8/P6P/3n4/2K5/R2B4/8 b - -");
	testcase_symmetry(&pos,"1rb1r1k1/2q2pb1/pp1p4/2n1pPPQ/Pn1BP3/1NN4R/1PP4P/R5K1 b - -");
	testcase_symmetry(&pos,"6k1/1b4p1/5p1p/pq3P2/1p1BP3/1P2QR1P/P1r3PK/8 w - -");
	testcase_symmetry(&pos,"8/5pk1/7p/3p1R2/p1p3P1/2P2K1P/1P1r4/8 w - -");
	testcase_symmetry(&pos,"6k1/p3pp2/6p1/7P/R7/b1q2P2/B1P1K2P/7R b - -");
	testcase_symmetry(&pos,"r7/1b4k1/pp1np1p1/3pq1NN/7P/4P3/PP4P1/1Q3RK1 b - -");
	testcase_symmetry(&pos,"4b3/2p4p/pp1bk3/2p3p1/2P5/PPB2PP1/7P/3K1N2 w - -");
	testcase_symmetry(&pos,"r1bqr1k1/ppp2ppp/3p4/4n3/2PN4/P1Q1P3/1PB2PPP/R4RK1 b - -");
	testcase_symmetry(&pos,"r4rk1/1ppqbppp/p2p1n2/8/1n1PP3/1Q3N2/PP1N1PPP/R1B1R1K1 b - -");
	testcase_symmetry(&pos,"r6k/1p4bp/1p1n1pp1/1B6/8/P4NP1/1P3P1P/2R3K1 w - -");
	testcase_symmetry(&pos,"r1b2r1k/pp3n1p/2p1p3/3Pnppq/3PP3/1P1N1PP1/P5BP/R1Q2RK1 w - -");
	testcase_symmetry(&pos,"2kr3r/1bpnqp2/1p2p3/p2p3p/P1PPBPp1/2P1P1P1/2QN2P1/1R2K2R w K -");
	testcase_symmetry(&pos,"8/1R6/3k4/2p5/2p1B3/5K2/8/8 w - -");
	testcase_symmetry(&pos,"1BR2rk1/pP1nbpp1/B2P2p1/8/8/8/1P4P1/3n2K1 b - -");
	testcase_symmetry(&pos,"r1b1k2r/1p1n1pp1/p6p/2p5/4Nb2/5NP1/PPP2P1P/1K1R1B1R b kq -");
	testcase_symmetry(&pos,"r1b2rk1/1p1n1pp1/p6p/2p5/4Nb2/3R1NP1/PPP2P1P/1K1R1B2 b - -");
	testcase_symmetry(&pos,"1kr5/1p1b2R1/p3p2Q/2bp3P/8/P1PB1P2/1P1K1P2/R6q b - -");
	testcase_symmetry(&pos,"rb3rk1/1p1RRpp1/p6p/r1p5/4Nb2/5NP1/PPP2P1P/1K3B2 b - -");
	testcase_symmetry(&pos,"5rk1/1pqn2pp/4pn2/p7/2P5/4PP2/1B2BP1P/3Q1RK1 w - -");
	testcase_symmetry(&pos,"3k1q2/p3p1p1/1p1nQ3/3P4/P2P4/B2P4/6KP/8 b - -");
	testcase_symmetry(&pos,"6k1/4R1P1/5P2/5K1p/7r/8/8/8 w - -");
	testcase_symmetry(&pos,"1n1q1rk1/4ppbp/3p1np1/1PpP4/4P3/2N2N2/3B1PPP/Q3K2R b K -");
	testcase_symmetry(&pos,"3q1rk1/4ppbp/1n1p1np1/1PpP4/2N1P3/5N2/3B1PPP/Q3K2R b K -");
	testcase_symmetry(&pos,"3q1rk1/4ppbp/1n1p1np1/1P1P4/4P3/2p1BN2/2N2PPP/Q3K2R b K -");
	testcase_symmetry(&pos,"N5r1/pQ6/3b1nkp/2q5/2Pp1p2/4nP2/PP1B2PP/1RR3K1 b - -");
	testcase_symmetry(&pos,"8/2kn2q1/B1p2pP1/P1P2p1p/3P2bP/3P1B2/1K1P1Q2/8 b - -");
	testcase_symmetry(&pos,"5nk1/3b1r2/2p1p3/1pPpP1qp/1P1Q4/6P1/4BN1P/R5K1 w - - 0 1");
}

void test_eval_material() {
	position pos;
	search_stats stats;

	set_pos(&pos,"8/k7/prb5/K7/QN6/8/8/8 b - - 0 1");
	int32_pair np = eval_nonpawn_material(&pos);
	assert(np.val1==queen_val+knight_val);
	assert(np.val2==rook_val+bishop_val);

	int32_pair p = eval_pawn_material(&pos);
	assert(p.val1==0);
	assert(p.val2==pawn_val);

	assert(eval(&pos,true,&stats)==pawn_val+rook_val+bishop_val-queen_val-knight_val);
}

void test_eval_scale() {
	int32 material = rook_val*2 + knight_val*2 + bishop_val*2 + queen_val;
	assert(100==eval_scale(100,material));

	assert(15==eval_scale(100,rook_val));
}

void test_evalfunc() {
	position pos;
	search_stats stats;

	reset_pos(&pos);
	assert(eval(&pos,false,&stats)==0);

	// ensure the score is from the player POV
	set_pos(&pos,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
	int32 score=eval(&pos,false,&stats);
	pos.player=WHITE;
	int32 score2=eval(&pos,false,&stats);
	assert(score==-score2);
}

void test_eval() {
	test_start_pos_is_0();
	test_eval_pawn();
	test_eval_knight();
	test_eval_bishop();
	test_eval_rook();
	test_eval_rook_on_7th();
	test_eval_connected_majors_7th();
	test_eval_rook_open_file();
	test_eval_rook_half_open_file();
	test_eval_queen();
	test_king_pst();
	test_king_endgame_pst();
	test_king_safety_middle_files();
	test_king_safety_kingside();
	test_king_safety_queenside();
	test_double_and_isolated_pawns();
	test_score1();
	test_score2();
	test_vertical_symmetry();
	test_eval_material();
	test_eval_scale();
	test_evalfunc();
}
