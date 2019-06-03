/*
 * extend__test.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: James
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "../src/globals.h"
#include "../src/defs.h"

void test_no_extension() {
    position pos;
    reset_pos(&pos);

    move e2e4 = to_move(PAWN,E2,E4);
    apply_move(&pos,e2e4,gundos);

    assert(0==extend(&pos,e2e4,false));
}

void test_extend_promotion() {
    position pos;
    set_pos(&pos,"k7/b1P5/KP6/6q1/8/8/8/4n3 w - -");

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move c7c8q = to_move(PAWN,C7,C8);
    set_promopiece(&c7c8q,QUEEN);

    assert(is_in_move_list(c7c8q,moves,endp));

    apply_move(&pos,c7c8q,gundos);

    assert(1==extend(&pos,c7c8q,true));
}

void test_white_push_to_7th() {
    position pos;
    set_pos(&pos,"k7/b1P5/KP6/6q1/8/8/8/4n3 w - -");

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move b6b7 = to_move(PAWN,B6,B7);

    assert(is_in_move_list(b6b7,moves,endp));

    apply_move(&pos,b6b7,gundos);
    assert(1==extend(&pos,b6b7,true));
}

void test_passed_pawn() {
    position pos;
    set_pos(&pos,"8/5ppp/8/5PPP/8/6k1/8/6K1 b - -");

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move g7g6 = to_move(PAWN,G7,G6);

    assert(is_in_move_list(g7g6,moves,endp));

    apply_move(&pos,g7g6,gundos);
    assert(0==extend(&pos,g7g6,false));
}

void test_passed_pawn2() {
    position pos;
    set_pos(&pos,"8/8/1PP2PbP/3r4/8/1Q5p/p5N1/k3K3 b - -");

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move d5d4 = to_move(ROOK,D5,D4);

    assert(is_in_move_list(d5d4,moves,endp));
    apply_move(&pos,d5d4,gundos);
    assert(0==extend(&pos,d5d4,false));
}

void test_black_push_to_2nd() {
    position pos;
    set_pos(&pos,"8/8/1PP2PbP/3r4/8/1Q5p/p5N1/k3K3 b - -");

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move h3h2 = to_move(PAWN,H3,H2);

    assert(is_in_move_list(h3h2,moves,endp));
    apply_move(&pos,h3h2,gundos);
    assert(1==extend(&pos,h3h2,false));
}

void test_passed_pawn3() {
    // Fischer-Larsen 71
    position pos;
    set_pos(&pos,"8/4kp2/6p1/7p/P7/2K3P1/7P/8 w - -");

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move a4a5 = to_move(PAWN,A4,A5);

    assert(is_in_move_list(a4a5,moves,endp));
    apply_move(&pos,a4a5,gundos);
    assert(0==extend(&pos,a4a5,false));
}

void test_passed_pawn4() {
    position pos;
    set_pos(&pos,"8/4kp2/6p1/7p/P7/2K3P1/7P/8 b - -");

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move f7f6 = to_move(PAWN,F7,F6);

    assert(is_in_move_list(f7f6,moves,endp));
    apply_move(&pos,f7f6,gundos);
    assert(0==extend(&pos,f7f6,false));
}


void test_passed_pawn5() {
    position pos;
    set_pos(&pos,"8/p3q1kp/1p2Pnp1/3pQ3/2pP4/1nP3N1/1B4PP/6K1 b - -");
    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move a7a6 = to_move(PAWN,A7,A6);

    assert(is_in_move_list(a7a6,moves,endp));
    apply_move(&pos,a7a6,gundos);
    assert(0==extend(&pos,a7a6,false));
}

void test_passed_pawn6() {
    position pos;
    set_pos(&pos,"8/p3q1kp/1p2Pnp1/3pQ3/2pP4/1nP3N1/1B4PP/6K1 b - -");
    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move b6b5 = to_move(PAWN,B6,B5);

    assert(is_in_move_list(b6b5,moves,endp));
    apply_move(&pos,b6b5,gundos);
    assert(0==extend(&pos,b6b5,false));
}

void test_newly_created_passed_pawn() {
    position pos;
    set_pos(&pos,"7k/8/6p1/4p3/5P2/8/8/7K w - -");

    move f4e5 = to_capture(PAWN,F4,E5,PAWN);

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    assert(is_in_move_list(f4e5,moves,endp));
    apply_move(&pos,f4e5,gundos);

    assert(0==extend(&pos,f4e5,false));
    undo_last_move(&pos,gundos);

    move f4f5 = to_move(PAWN,F4,F5);
    assert(is_in_move_list(f4f5,moves,endp));
    apply_move(&pos,f4f5,gundos);

    assert(0==extend(&pos,f4f5,false));
}

void test_recapture() {
    position pos;
    set_pos(&pos,"7k/8/8/3r4/3R4/3r4/8/6K1 w - -");

    move d4d5 = to_capture(ROOK,D4,D5,ROOK);

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    assert(is_in_move_list(d4d5,moves,endp));
    apply_move(&pos,d4d5,gundos);

    move d3d5 = to_capture(ROOK,D3,D5,ROOK);

    move moves2[MAX_MOVES_PER_PLY];
    move *endp2 = gen_legal_moves(moves2,&pos,true,true);
    assert(is_in_move_list(d3d5,moves2,endp2));

    assert(1==extend(&pos,d4d5,false));
}

void test_recapture2() {
    position pos;
    set_pos(&pos,"7k/8/8/3r4/3R4/3q4/8/6K1 w - -");

    move d4d5 = to_capture(ROOK,D4,D5,ROOK);

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    assert(is_in_move_list(d4d5,moves,endp));
    apply_move(&pos,d4d5,gundos);

    move d3d5 = to_capture(QUEEN,D3,D5,ROOK);

    move moves2[MAX_MOVES_PER_PLY];
    move *endp2 = gen_legal_moves(moves2,&pos,true,true);
    assert(is_in_move_list(d3d5,moves2,endp2));

    assert(1==extend(&pos,d3d5,false));
}

void test_recapture3() {
    position pos;
    set_pos(&pos,"7k/8/8/3b4/3R4/3q4/8/6K1 w - -");

    move d4d5 = to_capture(ROOK,D4,D5,BISHOP);

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    assert(is_in_move_list(d4d5,moves,endp));
    apply_move(&pos,d4d5,gundos);

    move d3d5 = to_capture(QUEEN,D3,D5,ROOK);

    move moves2[MAX_MOVES_PER_PLY];
    move *endp2 = gen_legal_moves(moves2,&pos,true,true);
    assert(is_in_move_list(d3d5,moves2,endp2));

    assert(0==extend(&pos,d3d5,false));
}

void test_extend() {
    test_no_extension();
    test_extend_promotion();
    test_white_push_to_7th();
    test_passed_pawn();
    test_passed_pawn2();
    //test_black_push_to_2nd();
    test_passed_pawn3();
    test_passed_pawn4();
    test_passed_pawn5();
    test_passed_pawn6();
    test_newly_created_passed_pawn();
    //test_recapture();
    //test_recapture2();
    //test_recapture3();
}
