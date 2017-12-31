/*
 * pos__test.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: james
 */

#include <cassert>
#include <cstring>
#include "../src/defs.h"
#include "../src/globals.h"

void test_reset_pos() {
	position pos;
	reset_pos(&pos);

	// first rank (white)
	assert(pos.piece[A1]==ROOK);
	assert(pos.piece[B1]==KNIGHT);
	assert(pos.piece[C1]==BISHOP);
	assert(pos.piece[D1]==QUEEN);
	assert(pos.piece[E1]==KING);
	assert(pos.piece[F1]==BISHOP);
	assert(pos.piece[G1]==KNIGHT);
	assert(pos.piece[H1]==ROOK);

	// back rank (black)
	assert(pos.piece[A8]==-ROOK);
	assert(pos.piece[B8]==-KNIGHT);
	assert(pos.piece[C8]==-BISHOP);
	assert(pos.piece[D8]==-QUEEN);
	assert(pos.piece[E8]==-KING);
	assert(pos.piece[F8]==-BISHOP);
	assert(pos.piece[G8]==-KNIGHT);
	assert(pos.piece[H8]==-ROOK);

	for (int32 sq=A8;sq<=H1;sq++) {
		int32 p = pos.piece[sq];
		rank_t r = get_rank((square_t)sq);
		if (r==RANK_8) { ;
		} else if (r==RANK_7) {
			assert(p==-PAWN);
		} else if (r==RANK_6  || r==RANK_5
				|| r==RANK_4 || r==RANK_3) {
			assert(p==NO_PIECE);
		} else if (r==RANK_2) {
			assert(p==PAWN);
		} else if (r==RANK_1) { ;
		} else {
			assert(false);
		}
	}

	assert(pos.player==WHITE);
	assert(pos.ep_sq==NO_SQUARE);
	assert(can_castle_wk(&pos));
	assert(can_castle_wq(&pos));
	assert(can_castle_bk(&pos));
	assert(can_castle_bq(&pos));
	assert(pos.move_counter==0);
	assert(pos.fifty_counter==0);
	assert(pos.white_king==E1);
	assert(pos.black_king==E8);
}

void test_add_piece() {
	position pos;
	reset_pos(&pos);

	add_piece(&pos,PAWN,E4);
	assert(pos.piece[E4]==PAWN);

	add_piece(&pos,-PAWN,E5);
	assert(pos.piece[E5]==-PAWN);

	add_piece(&pos,KNIGHT,F3);
	assert(pos.piece[F3]==KNIGHT);
}

void test_remove_piece() {
	position pos;
	reset_pos(&pos);

	remove_piece(&pos,E2);
	assert(pos.piece[E2]==NO_PIECE);

	remove_piece(&pos,H1);
	assert(pos.piece[H1]==NO_PIECE);

	remove_piece(&pos,E8);
	assert(pos.piece[E8]==NO_PIECE);
}

void test_equal_pos() {
	position pos1;
	reset_pos(&pos1);

	position pos2;
	reset_pos(&pos2);

	assert(equal_pos(&pos1,&pos2,true));

	add_piece(&pos2,PAWN,E4);
	assert(!equal_pos(&pos1,&pos2,true));
	remove_piece(&pos2,E4);
	assert(equal_pos(&pos1,&pos2,true));

	pos2.player=BLACK;
	assert(!equal_pos(&pos1,&pos2,true));
	reset_pos(&pos2);
	assert(equal_pos(&pos1,&pos2,true));

	pos2.ep_sq=E4;
	assert(!equal_pos(&pos1,&pos2,true));
	reset_pos(&pos2);
	assert(equal_pos(&pos1,&pos2,true));

	pos2.move_counter=1;
	assert(!equal_pos(&pos1,&pos2,true));
	reset_pos(&pos2);
	assert(equal_pos(&pos1,&pos2,true));

	pos2.fifty_counter=1;
	assert(!equal_pos(&pos1,&pos2,true));
	reset_pos(&pos2);
	assert(equal_pos(&pos1,&pos2,true));

	pos2.white_king=E4;
	assert(!equal_pos(&pos1,&pos2,true));
	pos2.white_king=E1;
	assert(equal_pos(&pos1,&pos2,true));

	pos2.black_king=E4;
	assert(!equal_pos(&pos1,&pos2,true));
	pos2.black_king=E8;
	assert(equal_pos(&pos1,&pos2,true));
}

void test_set_pos() {
	position pos1;
	reset_pos(&pos1);

	position pos2;
	assert(set_pos(&pos2,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
	assert(equal_pos(&pos1,&pos2,true));

	/////
	assert(set_pos(&pos2,"8/7p/5k2/5p2/p1p2P2/Pr1pPK2/1P1R3P/8 b - - 12 47"));
	for (int32 sq=A8;sq<=G7;sq++) {
		assert(pos2.piece[sq]==NO_PIECE);
	}
	assert(pos2.piece[H7]==-PAWN);
	for (int32 sq=A6;sq<=E6;sq++) {
		assert(pos2.piece[sq]==NO_PIECE);
	}
	assert(pos2.piece[F6]==-KING);
	assert(pos2.black_king==F6);
	for (int32 sq=G6;sq<=E5;sq++) {
		assert(pos2.piece[sq]==NO_PIECE);
	}
	assert(pos2.piece[F5]==-PAWN);
	assert(pos2.piece[G5]==NO_PIECE);
	assert(pos2.piece[H5]==NO_PIECE);
	assert(pos2.piece[A4]==-PAWN);
	assert(pos2.piece[B4]==NO_PIECE);
	assert(pos2.piece[C4]==-PAWN);
	assert(pos2.piece[D4]==NO_PIECE);
	assert(pos2.piece[E4]==NO_PIECE);
	assert(pos2.piece[F4]==PAWN);
	assert(pos2.piece[G4]==NO_PIECE);
	assert(pos2.piece[H4]==NO_PIECE);
	assert(pos2.piece[A3]==PAWN);
	assert(pos2.piece[B3]==-ROOK);
	assert(pos2.piece[C3]==NO_PIECE);
	assert(pos2.piece[D3]==-PAWN);
	assert(pos2.piece[E3]==PAWN);
	assert(pos2.piece[F3]==KING);
	assert(pos2.white_king==F3);
	assert(pos2.piece[G3]==NO_PIECE);
	assert(pos2.piece[H3]==NO_PIECE);
	assert(pos2.piece[A2]==NO_PIECE);
	assert(pos2.piece[B2]==PAWN);
	assert(pos2.piece[C2]==NO_PIECE);
	assert(pos2.piece[D2]==ROOK);
	assert(pos2.piece[E2]==NO_PIECE);
	assert(pos2.piece[F2]==NO_PIECE);
	assert(pos2.piece[G2]==NO_PIECE);
	assert(pos2.piece[H2]==PAWN);
	for (int32 sq=A1;sq<=H1;sq++) {
		assert(pos2.piece[sq]==NO_PIECE);
	}
	assert(pos2.player==BLACK);
	assert(pos2.ep_sq==NO_SQUARE);
	assert(pos2.castling_rights==0);
	assert(pos2.fifty_counter==12);
	assert(pos2.move_counter==93);

	//////
	assert(set_pos(&pos2,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"));
	assert(pos2.player==BLACK);
	assert(can_castle_wk(&pos2));
	assert(can_castle_wq(&pos2));
	assert(can_castle_bk(&pos2));
	assert(can_castle_bq(&pos2));
	assert(pos2.ep_sq==E3);
	assert(pos2.fifty_counter==0);
	assert(pos2.move_counter==1);
}

void test_remove_castling_availability() {
	position pos;
	set_pos(&pos,"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
	assert(can_castle_wk(&pos));
	assert(can_castle_wq(&pos));
	assert(can_castle_bk(&pos));
	assert(can_castle_bq(&pos));

	move mv = to_capture(ROOK,A1,A8,ROOK);
	remove_castling_availability(&pos,mv);
	assert(can_castle_wk(&pos));
	assert(!can_castle_wq(&pos));
	assert(can_castle_bk(&pos));
	assert(!can_castle_bq(&pos));

	mv = to_move(KING,E8,E7);
	remove_castling_availability(&pos,mv);
	assert(can_castle_wk(&pos));
	assert(!can_castle_wq(&pos));
	assert(!can_castle_bk(&pos));
	assert(!can_castle_bq(&pos));

	mv = to_move(ROOK,H1,H7);
	remove_castling_availability(&pos,mv);
	assert(pos.castling_rights==0);
}

void test_remove_rook_castling_availability() {
	position pos;
	reset_pos(&pos);
	assert(can_castle_wk(&pos));
	assert(can_castle_wq(&pos));
	assert(can_castle_bk(&pos));
	assert(can_castle_bq(&pos));

	remove_rook_castling_availability(&pos,A1);
	assert(can_castle_wk(&pos));
	assert(!can_castle_wq(&pos));
	assert(can_castle_bk(&pos));
	assert(can_castle_bq(&pos));

	remove_rook_castling_availability(&pos,H8);
	assert(can_castle_wk(&pos));
	assert(!can_castle_wq(&pos));
	assert(!can_castle_bk(&pos));
	assert(can_castle_bq(&pos));

	remove_rook_castling_availability(&pos,H8);
	assert(can_castle_wk(&pos));
	assert(!can_castle_wq(&pos));
	assert(!can_castle_bk(&pos));
	assert(can_castle_bq(&pos));

	remove_rook_castling_availability(&pos,H1);
	assert(!can_castle_wk(&pos));
	assert(!can_castle_wq(&pos));
	assert(!can_castle_bk(&pos));
	assert(can_castle_bq(&pos));

	remove_rook_castling_availability(&pos,A8);
	assert(pos.castling_rights==0);

}

void test_pos_consistency() {
	position pos;
	reset_pos(&pos);
	test_pos_consistency(&pos);
}

bool test_pos_consistency(position *pos) {

	assert(pos);

	// king placement
	square_t wkingsq=NO_SQUARE;
	for (int32 sq=0;sq<64;sq++) {
		if (pos->piece[sq]==KING) {
			wkingsq=(square_t)sq;
			break;
		}
	}
	if (wkingsq != pos->white_king) {
		print("wkingsq: %d, %d\n",wkingsq,pos->white_king);
		return false;
	}

	square_t bkingsq=NO_SQUARE;
	for (int32 sq=0;sq<64;sq++) {
		if (pos->piece[sq]==-KING) {
			bkingsq=(square_t)sq;
			break;
		}
	}
	if (bkingsq != pos->black_king) {
		print("bkingsq: %d, %d\n",bkingsq,pos->black_king);
		return false;
	}

	// ep square
	if (pos->ep_sq!=NO_SQUARE) {
		if (pos->player==BLACK) {
			assert(get_rank(pos->ep_sq)==RANK_3);
			assert(pos->piece[pos->ep_sq-8]==PAWN);
		} else {
			assert(get_rank(pos->ep_sq)==RANK_6);
			assert(pos->piece[pos->ep_sq+8]==-PAWN);
		}
	}

	// piece counts and bitmaps
	uint32 num_wpawns=0,num_wrooks=0,num_wknights=0,num_wbishops=0,num_wqueens=0;
	uint32 num_bpawns=0,num_brooks=0,num_bknights=0,num_bbishops=0,num_bqueens=0;

	uint64 bb_wpawns=0,bb_wrooks=0,bb_wknights=0,bb_wbishops=0,bb_wqueens=0,bb_wpieces=0;
	uint64 bb_bpawns=0,bb_brooks=0,bb_bknights=0,bb_bbishops=0,bb_bqueens=0,bb_bpieces=0;

	for (int32 sq=0;sq<64;sq++) {
		if (pos->piece[sq] > NO_PIECE) {
			bb_wpieces |= bb_squares[sq];
		} else if (pos->piece[sq] < NO_PIECE) {
			bb_bpieces |= bb_squares[sq];
		}

		if (pos->piece[sq]==PAWN) {
			num_wpawns++;
			bb_wpawns |= bb_squares[sq];
		} else if (pos->piece[sq]==-PAWN) {
			num_bpawns++;
			bb_bpawns |= bb_squares[sq];
		} else if (pos->piece[sq]==ROOK) {
			num_wrooks++;
			bb_wrooks |= bb_squares[sq];
		} else if (pos->piece[sq]==-ROOK) {
			num_brooks++;
			bb_brooks |= bb_squares[sq];
		} else if (pos->piece[sq]==KNIGHT) {
			num_wknights++;
			bb_wknights |= bb_squares[sq];
		} else if (pos->piece[sq]==-KNIGHT) {
			num_bknights++;
			bb_bknights |= bb_squares[sq];
		} else if (pos->piece[sq]==BISHOP) {
			num_wbishops++;
			bb_wbishops |= bb_squares[sq];
		} else if (pos->piece[sq]==-BISHOP) {
			num_bbishops++;
			bb_bbishops |= bb_squares[sq];
		} else if (pos->piece[sq]==QUEEN) {
			num_wqueens++;
			bb_wqueens |= bb_squares[sq];
		} else if (pos->piece[sq]==-QUEEN) {
			num_bqueens++;
			bb_bqueens |= bb_squares[sq];
		}
	}

	assert(bb_wpawns==pos->white_pawns);
	assert(get_num_bits(bb_wpawns)==num_wpawns);
	assert(num_wpawns==pos->piece_counts[WHITE][PAWN]);

	assert(bb_bpawns==pos->black_pawns);
	assert(get_num_bits(bb_bpawns)==num_bpawns);
	assert(num_bpawns==pos->piece_counts[BLACK][PAWN]);

	assert(bb_wknights==pos->white_knights);
	assert(get_num_bits(bb_wknights)==num_wknights);
	assert(num_wknights==pos->piece_counts[WHITE][KNIGHT]);

	assert(bb_bknights==pos->black_knights);
	assert(get_num_bits(bb_bknights)==num_bknights);
	assert(num_bknights==pos->piece_counts[BLACK][KNIGHT]);

	assert(bb_wbishops==pos->white_bishops);
	assert(get_num_bits(bb_wbishops)==num_wbishops);
	assert(num_wbishops==pos->piece_counts[WHITE][BISHOP]);

	assert(bb_bbishops==pos->black_bishops);
	assert(get_num_bits(bb_bbishops)==num_bbishops);
	assert(num_bbishops==pos->piece_counts[BLACK][BISHOP]);

	assert(bb_wrooks==pos->white_rooks);
	assert(get_num_bits(bb_wrooks)==num_wrooks);
	assert(num_wrooks==pos->piece_counts[WHITE][ROOK]);

	assert(bb_brooks==pos->black_rooks);
	assert(get_num_bits(bb_brooks)==num_brooks);
	assert(num_brooks==pos->piece_counts[BLACK][ROOK]);

	assert(bb_wqueens==pos->white_queens);
	assert(get_num_bits(bb_wqueens)==num_wqueens);
	assert(num_wqueens==pos->piece_counts[WHITE][QUEEN]);

	assert(bb_bqueens==pos->black_queens);
	assert(get_num_bits(bb_bqueens)==num_bqueens);
	assert(num_bqueens==pos->piece_counts[BLACK][QUEEN]);

	assert(bb_wpieces==pos->white_pieces);
	assert(bb_bpieces==pos->black_pieces);

	// assert casting rights make sense
	if (can_castle_bq(pos)) {
		assert(pos->piece[E8]==-KING);
		assert(pos->piece[A8]==-ROOK);
	}
	if (can_castle_bk(pos)) {
		assert(pos->piece[E8]==-KING);
		assert(pos->piece[H8]==-ROOK);
	}
	if (can_castle_wq(pos)) {
		assert(pos->piece[E1]==KING);
		assert(pos->piece[A1]==ROOK);
	}
	if (can_castle_wk(pos)) {
		assert(pos->piece[E1]==KING);
		assert(pos->piece[H1]==ROOK);
	}

	assert(pos->hash_key==build_hash_key(pos));
	assert(pos->pawn_key==build_pawn_key(pos));

	return true;
}

void test_is_empty_sq() {
	position pos;
	reset_pos(&pos);
	assert(is_empty_sq(&pos,D4));
	assert(!is_empty_sq(&pos,D8));
}

void test_is_enemy_and_friendly_occupied() {
	position pos;
	reset_pos(&pos);
	for (int32 sq=A8;sq <= H1; sq++) {
		int32 piece = pos.piece[sq];

		if (is_white_piece(piece)) {
			assert(is_friendly_occupied(&pos,(square_t)sq,WHITE));
			assert(!is_enemy_occupied(&pos,(square_t)sq,WHITE));
		} else if (is_black_piece(piece)) {
			assert(!is_friendly_occupied(&pos,(square_t)sq,WHITE));
			assert(is_enemy_occupied(&pos,(square_t)sq,WHITE));
		}
	}

	set_pos(&pos,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
	for (int32 sq=A8;sq <= H1; sq++) {
		int32 piece = pos.piece[sq];

		if (is_white_piece(piece)) {
			assert(!is_friendly_occupied(&pos,(square_t)sq,BLACK));
			assert(is_enemy_occupied(&pos,(square_t)sq,BLACK));
		} else if (is_black_piece(piece)) {
			assert(is_friendly_occupied(&pos,(square_t)sq,BLACK));
			assert(!is_enemy_occupied(&pos,(square_t)sq,BLACK));
		}
	}
}

void test_flip_vertical() {
	position pos;
	position save_pos;

	reset_pos(&pos);
	memcpy(&save_pos,&pos,sizeof(position));
	flip_board(&pos);
	flip_board(&pos);
	assert(equal_pos(&pos,&save_pos,true));

	///
	set_pos(&pos,"7r/R6p/2K4P/5k1P/2p4n/5p2/8/8 w - - 0 1");
	memcpy(&save_pos,&pos,sizeof(position));
	flip_board(&pos);
	flip_board(&pos);
	assert(equal_pos(&pos,&save_pos,true));
}

void test_is_checkmate() {
	position pos;

	// Fool's mate!
	set_pos(&pos,"rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq -");
	assert(is_checkmate(&pos));

	// Byrne vs Fischer
	set_pos(&pos,"1Q6/5pk1/2p3p1/1p2N2p/1b5P/1bn5/2r3P1/2K5 w - -");
	assert(is_checkmate(&pos));

	// Simple mate with rook
	set_pos(&pos,"3k2R1/8/3K4/8/8/8/8/8 b - -");
	assert(is_checkmate(&pos));

	// Starting position
	reset_pos(&pos);
	assert(!is_checkmate(&pos));

	set_pos(&pos,"8/8/8/6K1/8/1Q6/p7/k7 b - -");
	assert(!is_checkmate(&pos));
}

void test_is_stalemate() {
	position pos;

	// Burn vs Pilsbury, 1898
	set_pos(&pos,"5k2/5P2/5K2/8/8/8/8/8 b - -");
	assert(is_stalemate(&pos));

	set_pos(&pos,"kb6/8/1K6/8/8/8/8/8 b - - ");
	assert(!is_stalemate(&pos));
	set_pos(&pos,"kb5R/8/1K6/8/8/8/8/8 b - - ");
	assert(is_stalemate(&pos));

	set_pos(&pos,"8/8/8/8/8/2K5/1R6/k7 b - -");
	assert(is_stalemate(&pos));

	set_pos(&pos,"8/8/8/6K1/8/1Q6/p7/k7 b - -");
	assert(is_stalemate(&pos));

	set_pos(&pos,"rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq -");
	assert(!is_stalemate(&pos));
}

void test_is_zugzwang() {
	position pos;
	reset_pos(&pos);
	assert(!is_zugzwang(&pos));

	// just kings
	set_pos(&pos,"7k/8/8/8/8/8/8/7K w - - ");
	assert(is_zugzwang(&pos));

	// single white bishop
	set_pos(&pos,"7k/8/8/B7/8/8/8/7K b - - ");
	assert(is_zugzwang(&pos));

	// white pawn black knight
	set_pos(&pos,"7k/8/8/n7/P7/8/8/7K b - - ");
	assert(is_zugzwang(&pos));

	// white rook black knight
	set_pos(&pos,"7k/8/8/n7/R7/8/8/7K b - - ");
	assert(!is_zugzwang(&pos));

}

void test_pos() {
	test_pos_consistency();
	test_equal_pos();
	test_reset_pos();
	test_add_piece();
	test_remove_piece();
	test_set_pos();
	test_remove_rook_castling_availability();
	test_remove_castling_availability();
	test_is_empty_sq();
	test_is_enemy_and_friendly_occupied();
	test_flip_vertical();
	test_is_checkmate();
	test_is_stalemate();
	test_is_zugzwang();
}
