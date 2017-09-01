/*
 * hash__test.cpp
 *
 *  Created on: Feb 2, 2016
 *      Author: James
 */

#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"


bool contains_key(uint64 needle,uint64 haystack[],int n,int start) {
	for (int i=start;i<n;i++) {
		if (needle==haystack[i]) {
			return true;
		}
	}
	return false;
}

bool contains_key(uint64 needle,uint64 haystack[],int n) {
	return contains_key(needle,haystack,n,0);
}

void test_hash_keys1() {
	position pos;
	reset_pos(&pos);

	apply_move(&pos,to_move(PAWN,E2,E4),gundos);

	uint64 keys[20];
	int nkeys = 0;

	position pos2;
	set_pos(&pos2,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
	uint64 key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(PAWN,C7,C5),gundos);
	set_pos(&pos2,"rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(KNIGHT,G1,F3),gundos);
	set_pos(&pos2,"rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(QUEEN,D8,A5),gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/q1p5/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(BISHOP,F1,E2),gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/q1p5/4P3/5N2/PPPPBPPP/RNBQK2R b KQkq - 3 3");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_capture(QUEEN,A5,D2,PAWN),gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/2p5/4P3/5N2/PPPqBPPP/RNBQK2R w KQkq - 0 4");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_capture(KNIGHT,B1,D2,QUEEN),gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/2p5/4P3/5N2/PPPNBPPP/R1BQK2R b KQkq - 0 4");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(PAWN,C5,C4),gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/8/2p1P3/5N2/PPPNBPPP/R1BQK2R w KQkq - 0 5");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	move mv = to_move(KING,E1,G1);
	set_castle(&mv);
	apply_move(&pos,mv,gundos);
	set_pos(&pos2,"rnb1kbnr/pp1ppppp/8/8/2p1P3/5N2/PPPNBPPP/R1BQ1RK1 b kq - 0 5");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(KING,E8,D8),gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/2p1P3/5N2/PPPNBPPP/R1BQ1RK1 w - - 1 6");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(PAWN,B2,B4),gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/1Pp1P3/5N2/P1PNBPPP/R1BQ1RK1 b - b3 0 6");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	mv = to_move(PAWN,C4,B3);
	set_epcapture(&mv);
	apply_move(&pos,mv,gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/1p3N2/P1PNBPPP/R1BQ1RK1 w - - 0 7");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(ROOK,F1,E1),gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/1p3N2/P1PNBPPP/R1BQR1K1 b - - 1 7");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(PAWN,B3,B2),gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/5N2/PpPNBPPP/R1BQR1K1 w - - 0 8");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	apply_move(&pos,to_move(KING,G1,H1),gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/5N2/PpPNBPPP/R1BQR2K b - - 1 8");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
	keys[nkeys++] = key;

	mv = to_capture(PAWN,B2,A1,ROOK);
	set_promopiece(&mv,KNIGHT);
	apply_move(&pos,mv,gundos);
	set_pos(&pos2,"rnbk1bnr/pp1ppppp/8/8/4P3/5N2/P1PNBPPP/n1BQR2K w - - 0 9");
	key = build_hash_key(&pos);
	assert(key==build_hash_key(&pos2));
	assert(!contains_key(key,keys,nkeys));
}

// should be able to obtain an equal position using the French Defense or Petrov Defense
void test_hash_keys2() {
	position pos;
	reset_pos(&pos);

	position pos2;
	reset_pos(&pos2);

	uint64 keys[20]; int nkeys=0;
	uint64 keys2[20]; int nkeys2=0;

	assert(build_hash_key(&pos)==build_hash_key(&pos2));

	// step through French Defense with pos1
	apply_move(&pos,to_move(PAWN,E2,E4),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(PAWN,E7,E6),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(PAWN,D2,D4),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(PAWN,D7,D5),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_capture(PAWN,E4,D5,PAWN),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_capture(PAWN,E6,D5,PAWN),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(KNIGHT,G1,F3),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(KNIGHT,G8,F6),gundos);
	keys[nkeys++] = build_hash_key(&pos);

	// step through the Petrov Defense with pos2
	apply_move(&pos2,to_move(PAWN,E2,E4),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(PAWN,E7,E5),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(KNIGHT,G1,F3),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(KNIGHT,G8,F6),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_capture(KNIGHT,F3,E5,PAWN),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(PAWN,D7,D6),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(KNIGHT,E5,F3),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_capture(KNIGHT,F6,E4,PAWN),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(PAWN,D2,D3),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(KNIGHT,E4,F6),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(PAWN,D3,D4),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(PAWN,D6,D5),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);

	// Positions would be equal at this point, except for move history and fifty counter
	assert(!equal_pos(&pos,&pos2,true));
	assert(build_hash_key(&pos)==build_hash_key(&pos2));

	// by adding a pawn move we should be equal except move history and number of moves
	apply_move(&pos,to_move(PAWN,G2,G3),gundos);
	apply_move(&pos2,to_move(PAWN,G2,G3),gundos);

	assert(!equal_pos(&pos,&pos2,true));
	assert(build_hash_key(&pos)==build_hash_key(&pos2));

	// hash codes should be equal at beginning, move 1, move 7 and end only.
	for (int i=0;i<nkeys;i++) {
		if (i==0) {
			assert(keys2[i]==keys[i]);
			assert(!contains_key(keys[i],keys2,nkeys2,1));
		} else if (i==7) {
			assert(keys2[11]==keys[i]);
			assert(!contains_key(keys[i],keys2,nkeys2-1,0));
		} else {
			assert(!contains_key(keys[i],keys2,nkeys2));
		}
	}

}

/*
 * Should be able to obtain an equal position using the Queen's Gambit (d4,d5,c4,e6,Nc3,Nf6) and
 * the English Opening (c4,Nf6,Nc3,e6,d4,d5).
 */
void test_hash_keys3() {
	position pos;
	reset_pos(&pos);

	position pos2;
	reset_pos(&pos2);

	uint64 keys[20]; int nkeys=0;
	uint64 keys2[20]; int nkeys2=0;

	assert(build_hash_key(&pos)==build_hash_key(&pos2));

	// Go through Queen's Gambit with b1
	apply_move(&pos,to_move(PAWN,D2,D4),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(PAWN,D7,D5),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(PAWN,C2,C4),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(PAWN,E7,E6),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(KNIGHT,B1,C3),gundos);
	keys[nkeys++] = build_hash_key(&pos);
	apply_move(&pos,to_move(KNIGHT,G8,F6),gundos);
	keys[nkeys++] = build_hash_key(&pos);

	// Step through English Opening with b2
	apply_move(&pos2,to_move(PAWN,C2,C4),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(KNIGHT,G8,F6),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(KNIGHT,B1,C3),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(PAWN,E7,E6),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(PAWN,D2,D4),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);
	apply_move(&pos2,to_move(PAWN,D7,D5),gundos);
	keys2[nkeys2++] = build_hash_key(&pos2);

	// Positions would be equal at this point, except for move history, fifty counter and ep square
	assert(!equal_pos(&pos,&pos2,true));
	assert(build_hash_key(&pos)!=build_hash_key(&pos2));

	// by adding a pawn move we should be equal except move history
	apply_move(&pos,to_move(PAWN,G2,G3),gundos);
	apply_move(&pos2,to_move(PAWN,G2,G3),gundos);
	assert(equal_pos(&pos,&pos2,true));
	assert(build_hash_key(&pos)==build_hash_key(&pos2));

	// keys should be equal at beginning and end only.  Neither were
	// saved in list so lists should contain completely different codes
	for (int i=0;i<nkeys;i++) {
		assert(!contains_key(keys[i],keys2,nkeys2));
	}
}

void test_hash_val_lower_bound() {
	int32 depth = 14;
	int32 score = -109;
	move mv = to_move(PAWN,E2,E4);
	uint64 val = build_hash_val(LOWER_BOUND,depth,score,mv);

	assert(get_hash_entry_type(val)==LOWER_BOUND);
	assert(get_hash_entry_depth(val)==depth);
	assert(get_hash_entry_score(val)==score);
	assert(get_hash_entry_move(val)==mv);
}

void test_hash_val_upper_bound() {
	int32 depth = 45;
	move mv = 0;
	uint64 val = build_hash_val(UPPER_BOUND,depth,-CHECKMATE,mv);

	assert(get_hash_entry_type(val)==UPPER_BOUND);
	assert(get_hash_entry_depth(val)==depth);
	assert(get_hash_entry_score(val)==-CHECKMATE+500);
	assert(get_hash_entry_move(val)==mv);
}

void test_hash_val_exact_score() {
	int32 depth = 1;
	int32 score = -54;
	move mv = to_capture(PAWN,E7,F8,BISHOP);
	set_promopiece(&mv,QUEEN);
	uint64 val = build_hash_val(EXACT_SCORE,depth,score,mv);

	assert(get_hash_entry_type(val)==EXACT_SCORE);
	assert(get_hash_entry_depth(val)==depth);
	assert(get_hash_entry_score(val)==score);
	move found = get_hash_entry_move(val);
	assert(found==mv);
	assert(get_from_sq(mv)==E7);
	assert(get_to_sq(mv)==F8);
	assert(get_piece(mv)==PAWN);
	assert(is_capture(mv));
	assert(get_promopiece(found)==QUEEN);
}

void test_store_and_probe() {
	clear_hash_table();
	position pos;
	reset_pos(&pos);

	uint64 key = build_hash_key(&pos);

	// shouldn't be anything
	search_stats stats;
	uint64 val = get_hash_entry(key,&stats);
	assert(val==0);

	move mv = to_move(PAWN,E2,E4);
	store_hash_entry(key,build_hash_val(LOWER_BOUND,3,100,mv));
	val = get_hash_entry(key,&stats);
	assert(val > 0);
	assert(get_hash_entry_type(val)==LOWER_BOUND);
	assert(get_hash_entry_depth(val)==3);
	assert(get_hash_entry_score(val)==100);
	assert(get_hash_entry_move(val)==mv);

	// now make move and reprobe
	apply_move(&pos,mv,gundos);
	uint64 key2 = build_hash_key(&pos);
	uint64 val2 = get_hash_entry(key2,&stats);
	assert(val2==0);

	// finally undo and reprobe
	undo_last_move(&pos,gundos);
	key2 = build_hash_key(&pos);
	assert(key==key2);
	val2 = get_hash_entry(key2,&stats);
	assert(val==val2);
}

void test_with_mate_score() {
	clear_hash_table();
	position pos;
	set_pos(&pos,"5k2/6pp/p1qN4/1p1p4/3P4/2PKP2Q/PP3r2/3R4 b - -");

	uint64 key = build_hash_key(&pos);
	move mv = to_move(PAWN,C6,C4);
	store_hash_entry(key,build_hash_val(LOWER_BOUND,5,CHECKMATE-3,mv));

	search_stats stats;
	uint64 val = get_hash_entry(key,&stats);
	assert(val!=0);
	assert(get_hash_entry_move(val)==mv);

	// the idea here is to not deal with mate score adjustments!  Just say "it's at least a mate"
	assert(get_hash_entry_score(val)==CHECKMATE-500);
}

void test_transform_upper_bound_mate() {
	clear_hash_table();
	position pos;
	set_pos(&pos,"r4rk1/ppp2ppp/2n5/2bqp3/8/P2PB3/1PP1NPPP/R2Q1RK1 w - -");

	uint64 key = build_hash_key(&pos);
	move mv = to_move(ROOK,F1,E1);
	store_hash_entry(key,build_hash_val(UPPER_BOUND,5,CHECKMATE-3,mv));

	search_stats stats;
	uint64 val = get_hash_entry(key,&stats);
	assert(val!=0);
	assert(get_hash_entry_type(val)==MOVE_ONLY);
	assert(get_hash_entry_move(val)==mv);
	assert(get_hash_entry_depth(val)==5);
}

void test_transform_exact_score_mate() {
	clear_hash_table();
	position pos;
	set_pos(&pos,"3r2k1/p6p/2Q3p1/4q3/2P1p3/P3Pb2/1P3P1P/2K2BR1 b - - ");

	uint64 key = build_hash_key(&pos);
	move mv = to_move(ROOK,G8,F8);
	store_hash_entry(key,build_hash_val(EXACT_SCORE,5,CHECKMATE-7,mv));

	search_stats stats;
	uint64 val = get_hash_entry(key,&stats);
	assert(val!=0);
	assert(get_hash_entry_type(val)==LOWER_BOUND);
	assert(get_hash_entry_move(val)==mv);
	assert(get_hash_entry_score(val)==CHECKMATE-500);
	assert(get_hash_entry_depth(val)==5);
}

void test_transform_lower_bound_mated() {
	clear_hash_table();
	position pos;
	set_pos(&pos,"2rq1bk1/p4p1p/1p4p1/3b4/3B1Q2/8/P4PpP/3RR1K1 w - - ");

	uint64 key = build_hash_key(&pos);
	move mv = to_move(ROOK,D1,A1);
	store_hash_entry(key,build_hash_val(LOWER_BOUND,8,-CHECKMATE+10,mv));

	search_stats stats;
	uint64 val = get_hash_entry(key,&stats);
	assert(val!=0);
	assert(get_hash_entry_type(val)==MOVE_ONLY);
	assert(get_hash_entry_move(val)==mv);
	assert(get_hash_entry_depth(val)==8);
}

void test_transform_exact_score_mated() {
	clear_hash_table();
	position pos;
	set_pos(&pos,"4r1k1/5bpp/2p5/3pr3/8/1B3pPq/PPR2P2/2R2QK1 b - - ");

	uint64 key = build_hash_key(&pos);
	move mv = to_move(PAWN,G7,G6);
	store_hash_entry(key,build_hash_val(EXACT_SCORE,10,-CHECKMATE+2,mv));

	search_stats stats;
	uint64 val = get_hash_entry(key,&stats);
	assert(val!=0);
	assert(get_hash_entry_type(val)==UPPER_BOUND);
	assert(get_hash_entry_move(val)==mv);
	assert(get_hash_entry_score(val)==-CHECKMATE+500);
	assert(get_hash_entry_depth(val)==10);
}

void test_transform_upper_bound_mated() {
	clear_hash_table();
	position pos;
	set_pos(&pos,"r1b1k2r/1pp1q2p/p1n3p1/3QPp2/8/1BP3B1/P5PP/3R1RK1 w kq - ");

	uint64 key = build_hash_key(&pos);
	move mv = to_move(KING,G1,H1);
	store_hash_entry(key,build_hash_val(UPPER_BOUND,7,-CHECKMATE+8,mv));

	search_stats stats;
	uint64 val = get_hash_entry(key,&stats);
	assert(val!=0);
	assert(get_hash_entry_type(val)==UPPER_BOUND);
	assert(get_hash_entry_move(val)==mv);
	assert(get_hash_entry_score(val)==-CHECKMATE+500);
	assert(get_hash_entry_depth(val)==7);
}

void test_clear() {
	clear_hash_table();
	position pos;
	set_pos(&pos,"3qrrk1/1pp2pp1/1p2bn1p/5N2/2P5/P1P3B1/1P4PP/2Q1RRK1 w - - ");

	uint64 key = build_hash_key(&pos);
	move mv = to_move(KNIGHT,F5,D4);
	store_hash_entry(key,build_hash_val(UPPER_BOUND,4,-93,mv));

	search_stats stats;
	uint64 val = get_hash_entry(key,&stats);
	assert(val!=0);
	assert(get_hash_entry_type(val)==UPPER_BOUND);
	assert(get_hash_entry_move(val)==mv);
	assert(get_hash_entry_score(val)==-93);
	assert(get_hash_entry_depth(val)==4);

	clear_hash_table();
	val = get_hash_entry(key,&stats);
	assert(val==0);
}

void test_overwrite() {
	clear_hash_table();
	position pos;
	set_pos(&pos,"8/k7/p7/3Qp2P/n1P5/3KP3/1q6/8 b - - ");

	uint64 key = build_hash_key(&pos);
	move mv = to_move(PAWN,E5,E4);
	store_hash_entry(key,build_hash_val(LOWER_BOUND,5,1001,mv));

	search_stats stats;
	uint64 val = get_hash_entry(key,&stats);
	assert(val!=0);
	assert(get_hash_entry_type(val)==LOWER_BOUND);
	assert(get_hash_entry_move(val)==mv);
	assert(get_hash_entry_score(val)==1001);
	assert(get_hash_entry_depth(val)==5);

	// overwrite with different score/depth
	store_hash_entry(key,build_hash_val(LOWER_BOUND,6,900,mv));
	val = get_hash_entry(key,&stats);
	assert(val!=0);
	assert(get_hash_entry_type(val)==LOWER_BOUND);
	assert(get_hash_entry_move(val)==mv);
	assert(get_hash_entry_score(val)==900);
	assert(get_hash_entry_depth(val)==6);

	// use different key to store new entry
	uint64 key2 = ~key;

	uint64 val2 = get_hash_entry(key2,&stats);
	assert(val2==0);
	store_hash_entry(key2,build_hash_val(LOWER_BOUND,7,800,mv));
	val2 = get_hash_entry(key2,&stats);
	assert(val2 != 0);

	// make sure we didn't overwrite earlier entry
	assert(val==get_hash_entry(key,&stats));
	assert(val2==get_hash_entry(key2,&stats));
	assert(val != val2);
}

void test_hash() {
	test_hash_keys1();
	test_hash_keys2();
	test_hash_keys3();
	test_hash_val_lower_bound();
	test_hash_val_upper_bound();
	test_hash_val_exact_score();
	test_store_and_probe();
	test_with_mate_score();
	test_transform_upper_bound_mate();
	test_transform_exact_score_mate();
	test_transform_lower_bound_mated();
	test_transform_exact_score_mated();
	test_transform_upper_bound_mated();
	test_clear();
	test_overwrite();
}

