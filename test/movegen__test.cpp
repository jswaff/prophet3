/*
 * movegen__test.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: james
 */

#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"


void test_gen_knight_moves() {
    position pos;
    reset_pos(&pos);

    move moves[10];
    move *mp = gen_knight_moves(moves,&pos,B1,true,true);
    mp = gen_knight_moves(mp,&pos,G1,true,true);

    assert((mp - moves) == 4);
    assert(is_in_move_list(to_move(KNIGHT,B1,A3),moves,mp));
    assert(is_in_move_list(to_move(KNIGHT,B1,C3),moves,mp));
    assert(is_in_move_list(to_move(KNIGHT,G1,F3),moves,mp));
    assert(is_in_move_list(to_move(KNIGHT,G1,H3),moves,mp));
}

void test_gen_knight_captures() {
    position pos;
    set_pos(&pos,"4k3/8/3P1p2/8/4N3/8/8/4K3 w - - 0 1");

    move moves[10];
    move *mp = gen_knight_moves(moves,&pos,E4,true,false);

    assert((mp-moves)==1);
    assert(is_in_move_list(to_capture(KNIGHT,E4,F6,PAWN),moves,mp));
}

void test_gen_knight_noncaptures() {
    position pos;
    set_pos(&pos,"4k3/8/3P1p2/8/4N3/8/8/4K3 w - - 0 1");

    move moves[10];
    move *mp = gen_knight_moves(moves,&pos,E4,false,true);

    assert((mp-moves)==6);
    assert(is_in_move_list(to_move(KNIGHT,E4,G5),moves,mp));
    assert(is_in_move_list(to_move(KNIGHT,E4,G3),moves,mp));
    assert(is_in_move_list(to_move(KNIGHT,E4,F2),moves,mp));
    assert(is_in_move_list(to_move(KNIGHT,E4,D2),moves,mp));
    assert(is_in_move_list(to_move(KNIGHT,E4,C3),moves,mp));
    assert(is_in_move_list(to_move(KNIGHT,E4,C5),moves,mp));
}

void test_gen_bishop_moves() {
    position pos;
    set_pos(&pos,"4k3/2p3P1/8/4B3/4B3/3p1P2/8/4K3 w - - 0 1");

    move moves[20];
    move *mp = gen_bishop_moves(moves,&pos,E5,true,true);
    mp = gen_bishop_moves(mp,&pos,E4,true,true);

    assert((mp - moves) == 18);
    assert(is_in_move_list(to_move(BISHOP,E5,F6),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,F4),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,G3),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,H2),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,D4),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,C3),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,B2),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,A1),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,D6),moves,mp));
    assert(is_in_move_list(to_capture(BISHOP,E5,C7,PAWN),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,F5),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,G6),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,H7),moves,mp));
    assert(is_in_move_list(to_capture(BISHOP,E4,D3,PAWN),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,D5),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,C6),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,B7),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,A8),moves,mp));
}

void test_gen_bishop_captures() {
    position pos;
    set_pos(&pos,"4k3/2p3P1/8/4B3/4B3/3p1P2/8/4K3 w - - 0 1");

    move moves[20];
    move *mp = gen_bishop_moves(moves,&pos,E5,true,false);
    mp = gen_bishop_moves(mp,&pos,E4,true,false);

    assert((mp - moves) == 2);
    assert(is_in_move_list(to_capture(BISHOP,E5,C7,PAWN),moves,mp));
    assert(is_in_move_list(to_capture(BISHOP,E4,D3,PAWN),moves,mp));
}

void test_gen_bishop_noncaptures() {
    position pos;
    set_pos(&pos,"4k3/2p3P1/8/4B3/4B3/3p1P2/8/4K3 w - - 0 1");

    move moves[20];
    move *mp = gen_bishop_moves(moves,&pos,E5,false,true);
    mp = gen_bishop_moves(mp,&pos,E4,false,true);

    assert((mp - moves) == 16);
    assert(is_in_move_list(to_move(BISHOP,E5,F6),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,F4),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,G3),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,H2),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,D4),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,C3),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,B2),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,A1),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E5,D6),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,F5),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,G6),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,H7),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,D5),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,C6),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,B7),moves,mp));
    assert(is_in_move_list(to_move(BISHOP,E4,A8),moves,mp));
}

void test_gen_rook_moves() {
    position pos;
    reset_pos(&pos);

    move moves[20];
    move *mp = gen_rook_moves(moves,&pos,H1,true,true);
    assert((mp - moves) == 0);

    set_pos(&pos,"8/8/3k1p2/8/3K4/8/1R3r2/8 b - - 0 1");

    mp = gen_rook_moves(moves,&pos,F2,true,true);
    assert((mp - moves) == 10);

    assert(is_in_move_list(to_move(ROOK,F2,F3),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,F4),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,F5),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,F1),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,G2),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,H2),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,E2),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,D2),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,C2),moves,mp));
    assert(is_in_move_list(to_capture(ROOK,F2,B2,ROOK),moves,mp));
}

void test_gen_rook_captures() {
    position pos;
    reset_pos(&pos);

    move moves[20];
    move *mp = gen_rook_moves(moves,&pos,H1,true,false);
    assert((mp - moves) == 0);

    set_pos(&pos,"8/8/3k1p2/8/3K4/8/1R3r2/8 b - - 0 1");

    mp = gen_rook_moves(moves,&pos,F2,true,false);
    assert((mp - moves) == 1);

    assert(is_in_move_list(to_capture(ROOK,F2,B2,ROOK),moves,mp));
}

void test_gen_rook_noncaptures() {
    position pos;
    reset_pos(&pos);

    move moves[20];
    move *mp = gen_rook_moves(moves,&pos,H1,false,true);
    assert((mp - moves) == 0);

    set_pos(&pos,"8/8/3k1p2/8/3K4/8/1R3r2/8 b - - 0 1");

    mp = gen_rook_moves(moves,&pos,F2,false,true);
    assert((mp - moves) == 9);

    assert(is_in_move_list(to_move(ROOK,F2,F3),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,F4),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,F5),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,F1),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,G2),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,H2),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,E2),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,D2),moves,mp));
    assert(is_in_move_list(to_move(ROOK,F2,C2),moves,mp));
}

void test_gen_queen_moves() {
    position pos;
    set_pos(&pos,"8/8/3bk3/8/8/2K3Q1/8/8 w - - 0 1");

    move moves[20];
    move *mp = gen_queen_moves(moves,&pos,G3,true,true);

    assert((mp - moves) == 18);
    assert(is_in_move_list(to_move(QUEEN,G3,G4),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G5),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G6),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G7),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G8),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,H4),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,H3),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,H2),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G2),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G1),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,F2),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,E1),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,F3),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,E3),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,D3),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,F4),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,E5),moves,mp));
    assert(is_in_move_list(to_capture(QUEEN,G3,D6,BISHOP),moves,mp));
}

void test_gen_queen_captures() {
    position pos;
    set_pos(&pos,"8/8/3bk3/8/8/2K3Q1/8/8 w - - 0 1");

    move moves[20];
    move *mp = gen_queen_moves(moves,&pos,G3,true,false);

    assert((mp - moves) == 1);
    assert(is_in_move_list(to_capture(QUEEN,G3,D6,BISHOP),moves,mp));
}

void test_gen_queen_noncaptures() {
    position pos;
    set_pos(&pos,"8/8/3bk3/8/8/2K3Q1/8/8 w - - 0 1");

    move moves[20];
    move *mp = gen_queen_moves(moves,&pos,G3,false,true);

    assert((mp - moves) == 17);
    assert(is_in_move_list(to_move(QUEEN,G3,G4),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G5),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G6),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G7),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G8),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,H4),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,H3),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,H2),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G2),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,G1),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,F2),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,E1),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,F3),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,E3),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,D3),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,F4),moves,mp));
    assert(is_in_move_list(to_move(QUEEN,G3,E5),moves,mp));
}

void test_gen_king_moves() {
    position pos;

    set_pos(&pos,"8/8/3k4/2n1P3/8/8/3rP3/R3K2R b KQ - 0 1");
    move moves[20];
    move *mp = gen_king_moves(moves,&pos,true,true);
    assert((mp - moves) == 7);
    assert(is_in_move_list(to_move(KING,D6,D7),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,E7),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,E6),moves,mp));
    assert(is_in_move_list(to_capture(KING,D6,E5,PAWN),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,D5),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,C6),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,C7),moves,mp));

    // flip sides
    set_pos(&pos,"8/8/3k4/2n1P3/8/8/3rP3/RN2K2R w KQ - 0 1");
    mp = gen_king_moves(moves,&pos,true,true);
    assert((mp - moves)==5);
    assert(is_in_move_list(to_move(KING,E1,F2),moves,mp));
    assert(is_in_move_list(to_move(KING,E1,F1),moves,mp));
    assert(is_in_move_list(to_move(KING,E1,D1),moves,mp)); // NOTE: illegal move
    assert(is_in_move_list(to_capture(KING,E1,D2,ROOK),moves,mp));
    move mv = to_move(KING,E1,G1);
    set_castle(&mv);
    assert(is_in_move_list(mv,moves,mp));

    // move to a corner
    set_pos(&pos,"k7/8/8/8/8/8/8/7K w - - 0 1");
    mp = gen_king_moves(moves,&pos,true,true);
    assert((mp - moves)==3);
    assert(is_in_move_list(to_move(KING,H1,G1),moves,mp));
    assert(is_in_move_list(to_move(KING,H1,G2),moves,mp));
    assert(is_in_move_list(to_move(KING,H1,H2),moves,mp));

    set_pos(&pos,"k7/8/8/8/8/8/8/7K b - - 0 1");
    mp = gen_king_moves(moves,&pos,true,true);
    assert((mp - moves)==3);
    assert(is_in_move_list(to_move(KING,A8,A7),moves,mp));
    assert(is_in_move_list(to_move(KING,A8,B7),moves,mp));
    assert(is_in_move_list(to_move(KING,A8,B8),moves,mp));

    // cannot castle while in check
    set_pos(&pos,"3k4/8/8/2N1P3/7q/8/4P3/R3K2R w KQ - 0 1");
    mp = gen_king_moves(moves,&pos,true,true);
    assert((mp - moves)==4);
    assert(is_in_move_list(to_move(KING,E1,F1),moves,mp));
    // e1f2 stays in check, but that's handled separately.
    assert(is_in_move_list(to_move(KING,E1,F2),moves,mp));
    assert(is_in_move_list(to_move(KING,E1,D1),moves,mp));
    assert(is_in_move_list(to_move(KING,E1,D2),moves,mp));


    // cannot castle if king will passed through attacked square
    set_pos(&pos,"r3k2r/8/8/8/8/5Q2/8/4K3 b kq - 0 1");
    mp = gen_king_moves(moves,&pos,true,true);
    assert((mp - moves)==6);
    assert(is_in_move_list(to_move(KING,E8,F8),moves,mp));
    assert(is_in_move_list(to_move(KING,E8,F7),moves,mp));
    assert(is_in_move_list(to_move(KING,E8,E7),moves,mp));
    assert(is_in_move_list(to_move(KING,E8,D7),moves,mp));
    assert(is_in_move_list(to_move(KING,E8,D8),moves,mp));
    mv = to_move(KING,E8,C8);
    set_castle(&mv);
    assert(is_in_move_list(mv,moves,mp)); // queenside ok, not kingside
}

void test_gen_king_captures() {
    position pos;

    set_pos(&pos,"8/8/3k4/2n1P3/8/8/3rP3/R3K2R b KQ - 0 1");
    move moves[20];
    move *mp = gen_king_moves(moves,&pos,true,false);
    assert((mp - moves) == 1);
    assert(is_in_move_list(to_capture(KING,D6,E5,PAWN),moves,mp));

    // flip sides
    set_pos(&pos,"8/8/3k4/2n1P3/8/8/3rP3/RN2K2R w KQ - 0 1");
    mp = gen_king_moves(moves,&pos,true,false);
    assert((mp - moves)==1);
    assert(is_in_move_list(to_capture(KING,E1,D2,ROOK),moves,mp));

    // cannot castle if king will passed through attacked square
    set_pos(&pos,"r3k2r/8/8/8/8/5Q2/8/4K3 b kq - 0 1");
    mp = gen_king_moves(moves,&pos,true,false);
    assert((mp - moves)==0);
}

void test_gen_king_noncaptures() {
    position pos;

    set_pos(&pos,"8/8/3k4/2n1P3/8/8/3rP3/R3K2R b KQ - 0 1");
    move moves[20];
    move *mp = gen_king_moves(moves,&pos,false,true);
    assert((mp - moves) == 6);
    assert(is_in_move_list(to_move(KING,D6,D7),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,E7),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,E6),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,D5),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,C6),moves,mp));
    assert(is_in_move_list(to_move(KING,D6,C7),moves,mp));

    // flip sides
    set_pos(&pos,"8/8/3k4/2n1P3/8/8/3rP3/RN2K2R w KQ - 0 1");
    mp = gen_king_moves(moves,&pos,false,true);
    assert((mp - moves)==4);
    assert(is_in_move_list(to_move(KING,E1,F2),moves,mp));
    assert(is_in_move_list(to_move(KING,E1,F1),moves,mp));
    assert(is_in_move_list(to_move(KING,E1,D1),moves,mp)); // NOTE: illegal move
    move mv = to_move(KING,E1,G1);
    set_castle(&mv);
    assert(is_in_move_list(mv,moves,mp));
}

void test_gen_pawn_moves() {
    position pos;
    set_pos(&pos,"2b1k3/PP6/8/3pP3/4P3/8/6P1/4K3 w - d6 0 1");
    move moves[20],*mp,mv;

    mp = gen_pawn_moves(moves,&pos,true,true);

    assert((mp - moves)==17);
    assert(is_in_move_list(to_move(PAWN,G2,G3),moves,mp));
    assert(is_in_move_list(to_move(PAWN,G2,G4),moves,mp));

    mv = to_move(PAWN,A7,A8);
    set_promopiece(&mv,QUEEN);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,ROOK);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,BISHOP);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,KNIGHT);
    assert(is_in_move_list(mv,moves,mp));

    mv = to_move(PAWN,B7,B8);
    set_promopiece(&mv,QUEEN);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,ROOK);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,BISHOP);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,KNIGHT);
    assert(is_in_move_list(mv,moves,mp));

    mv = to_capture(PAWN,B7,C8,BISHOP);
    set_promopiece(&mv,QUEEN);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,ROOK);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,BISHOP);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,KNIGHT);
    assert(is_in_move_list(mv,moves,mp));

    mv = to_capture(PAWN,E4,D5,PAWN);
    assert(is_in_move_list(mv,moves,mp));

    assert(is_in_move_list(to_move(PAWN,E5,E6),moves,mp));
    mv = to_move(PAWN,E5,D6);
    set_epcapture(&mv);
    assert(is_in_move_list(mv,moves,mp));

    // from black
    set_pos(&pos,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    mp = gen_pawn_moves(moves,&pos,true,true);

    assert((mp - moves)==16);
    assert(is_in_move_list(to_move(PAWN,D7,D6),moves,mp));
    assert(is_in_move_list(to_move(PAWN,D7,D5),moves,mp));
}

void test_gen_pawn_captures() {
    position pos;
    set_pos(&pos,"2b1k3/PP6/8/3pP3/4P3/8/6P1/4K3 w - d6 0 1");
    move moves[20],*mp,mv;

    mp = gen_pawn_moves(moves,&pos,true,false);

    assert((mp - moves)==14);

    mv = to_move(PAWN,A7,A8);
    set_promopiece(&mv,QUEEN);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,ROOK);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,BISHOP);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,KNIGHT);
    assert(is_in_move_list(mv,moves,mp));

    mv = to_move(PAWN,B7,B8);
    set_promopiece(&mv,QUEEN);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,ROOK);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,BISHOP);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,KNIGHT);
    assert(is_in_move_list(mv,moves,mp));

    mv = to_capture(PAWN,B7,C8,BISHOP);
    set_promopiece(&mv,QUEEN);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,ROOK);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,BISHOP);
    assert(is_in_move_list(mv,moves,mp));
    set_promopiece(&mv,KNIGHT);
    assert(is_in_move_list(mv,moves,mp));

    mv = to_capture(PAWN,E4,D5,PAWN);
    assert(is_in_move_list(mv,moves,mp));

    mv = to_move(PAWN,E5,D6);
    set_epcapture(&mv);
    assert(is_in_move_list(mv,moves,mp));

    // from black
    set_pos(&pos,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    mp = gen_pawn_moves(moves,&pos,true,false);

    assert((mp - moves)==0);
}

void test_gen_pawn_noncaps() {
    position pos;
    set_pos(&pos,"2b1k3/PP6/8/3pP3/4P3/8/6P1/4K3 w - d6 0 1");
    move moves[20],*mp;

    mp = gen_pawn_moves(moves,&pos,false,true);

    assert((mp - moves)==3);
    assert(is_in_move_list(to_move(PAWN,G2,G3),moves,mp));
    assert(is_in_move_list(to_move(PAWN,G2,G4),moves,mp));
    assert(is_in_move_list(to_move(PAWN,E5,E6),moves,mp));
}


void test_gen_moves() {
    position pos;
    reset_pos(&pos);
    move moves[50],*mp;

    mp = gen_moves(moves,&pos,true,true);
    assert((mp-moves)==20);

    set_pos(&pos,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    mp = gen_moves(moves,&pos,true,true);
    assert((mp-moves)==20);
}

void test_gen_legal_moves() {
    position pos;
    set_pos(&pos,"7k/4Bp1p/8/8/8/8/8/1K4R1 b - - 0 1");
    move moves[50],*mp;
    mp = gen_legal_moves(moves,&pos,true,true);
    assert(num_moves(moves,mp,true,true)==4);

    set_pos(&pos,"rnb1k2r/ppq1n3/2pppp2/6pp/2PP3N/P2BP3/2PB1PPP/R2Q1RK1 w kq - -");
    assert(is_legal_move(to_move(KNIGHT,H4,G6),&pos));

    set_pos(&pos,"r2r2k1/1p2p1b1/1pp1b1pp/4P2n/PP3p1p/2P2N2/5PP1/R3KBNR b KQ b3 -");
    assert(is_legal_move(to_move(BISHOP,E6,B3),&pos));
}

void test_movegen() {
    test_gen_knight_moves();
    test_gen_knight_captures();
    test_gen_knight_noncaptures();
    test_gen_bishop_moves();
    test_gen_bishop_captures();
    test_gen_bishop_noncaptures();
    test_gen_rook_moves();
    test_gen_rook_captures();
    test_gen_rook_noncaptures();
    test_gen_queen_moves();
    test_gen_queen_captures();
    test_gen_queen_noncaptures();
    test_gen_king_moves();
    test_gen_king_captures();
    test_gen_king_noncaptures();
    test_gen_pawn_moves();
    test_gen_pawn_captures();
    test_gen_pawn_noncaps();
    test_gen_moves();
    test_gen_legal_moves();
}

