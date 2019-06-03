/*
 * next__test.cpp
 *
 *  Created on: Feb 10, 2016
 *      Author: James
 */

#include <cassert>
#include "../src/globals.h"
#include "../src/defs.h"

void test_best_at_top() {
    position pos;
    reset_pos(&pos);

    move moves[20];
    move *endp = gen_legal_moves(moves,&pos,true,true);
    assert(endp-moves==20);

    set_move_score(moves+4,1000);
    set_move_score(moves+11,900);
    set_move_score(moves+7,-1500);
    move mv1 = moves[4];
    move mv2 = moves[11];
    move mv3 = moves[7];

    int i=0;
    for (move *mp=moves;mp<endp;mp++) {
        if (*mp != mv1 && *mp != mv2 && *mp != mv3) {
            assert(get_move_score(mp)==0);
        }
        best_at_top(mp,endp);

        if (i==0) {
            assert(*mp==mv1);
        } else if (i==1) {
            assert(*mp==mv2);
        } else if (i==19) {
            assert(*mp==mv3);
        }
        i++;
    }
}

void test_place_move_on_top() {
    move moves[5];
    move e2e3 = moves[0] = to_move(PAWN,E2,E3);
    move e2e4 = moves[1] = to_move(PAWN,E2,E4);
    moves[2] = BADMOVE;
    move a2a3 = moves[3] = to_move(PAWN,A2,A3);
    move g1f3 = moves[4] = to_move(KNIGHT,G1,F3);

    assert(!place_move_on_top(moves,moves+5,BADMOVE));
    assert(!place_move_on_top(moves,moves+5,to_move(PAWN,B1,B2)));
    assert(place_move_on_top(moves,moves+5,a2a3));
    assert(moves[0]==a2a3);
    assert(moves[3]==e2e3);

    assert(place_move_on_top(moves+1,moves+5,g1f3));
    assert(moves[1]==g1f3);
    assert(moves[4]==e2e4);
}

void test_pv() {
    position pos;
    set_pos(&pos,"b2b1r1k/3R1ppp/4qP2/4p1PQ/4P3/5B2/4N1K1/8 w - - ");

    move moves[MAX_MOVES_PER_PLY];
    gen_legal_moves(moves,&pos,true,true);

    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = moves[5];
    mo_dto.hash_move = 0;
    mo_dto.killer1 = 0;
    mo_dto.killer2 = 0;
    mo_dto.next_stage = PVMOVE;

    move *nextp;
    assert(next(&pos,&nextp,&mo_dto));
    assert(*nextp==mo_dto.pv_move);
}

void test_hash_next() {
    position pos;
    set_pos(&pos,"1R6/1brk2p1/4p2p/p1P1Pp2/P7/6P1/1P4P1/2R3K1 w - - ");

    move moves[MAX_MOVES_PER_PLY];
    gen_legal_moves(moves,&pos,true,true);

    // randomly make the 5th move the hash move
    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = 0;
    mo_dto.hash_move = moves[5];
    mo_dto.killer1 = 0;
    mo_dto.killer2 = 0;
    mo_dto.next_stage = PVMOVE;

    move *nextp;
    assert(next(&pos,&nextp,&mo_dto));
    assert(*nextp==mo_dto.hash_move);
}

void test_pv_then_hash() {
    position pos;
    set_pos(&pos,"6k1/p4p1p/1p3np1/2q5/4p3/4P1N1/PP3PPP/3Q2K1 w - - ");

    move moves[MAX_MOVES_PER_PLY];
    gen_legal_moves(moves,&pos,true,true);

    // make move 4 the PV move and move 2 the hash move
    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = moves[4];
    mo_dto.hash_move = moves[2];
    mo_dto.killer1 = 0;
    mo_dto.killer2 = 0;
    mo_dto.next_stage = PVMOVE;

    move *nextp;
    assert(next(&pos,&nextp,&mo_dto));
    assert(*nextp==mo_dto.pv_move);

    assert(next(&pos,&nextp,&mo_dto));
    assert(*nextp==mo_dto.hash_move);
}

void test_pv_and_hash_same_move() {
    position pos;
    reset_pos(&pos);

    move moves[MAX_MOVES_PER_PLY];
    gen_legal_moves(moves,&pos,true,true);

    // make move 4 the PV move and move 2 the hash move
    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = moves[7];
    mo_dto.hash_move = mo_dto.pv_move;
    mo_dto.killer1 = 0;
    mo_dto.killer2 = 0;
    mo_dto.caps_only = false;
    mo_dto.next_stage = PVMOVE;

    move *nextp;
    assert(next(&pos,&nextp,&mo_dto));
    assert(*nextp==mo_dto.pv_move);

    for (int i=1;i<20;i++) {
        assert(next(&pos,&nextp,&mo_dto));
        assert(*nextp!=mo_dto.pv_move);
    }

    // 21st call should return false
    assert(!next(&pos,&nextp,&mo_dto));
}

void test_promotions() {
    position pos;
    set_pos(&pos,"8/4Pk1p/6p1/1r6/8/5N2/2B2PPP/b5K1 w - - ");

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

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

    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = 0;
    mo_dto.hash_move = 0;
    mo_dto.killer1 = 0;
    mo_dto.killer2 = 0;
    mo_dto.next_stage = PVMOVE;

    move *nextp;
    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==e7e8q);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==e7e8r);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==e7e8b);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==e7e8n);

    // NOTE: C2G6 is a losing capture - in SEE it would be at bottom
    /*for (int i=5;i<24;i++) {
        assert(next(&pos,&nextp,&mo_dto));
    }*/

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==c2g6);
}

void test_pv_then_hash_then_captures() {
    position pos;
    set_pos(&pos,"6R1/kp6/8/1KpP4/8/8/8/6B1 w - c6");

    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move d5c6 = to_capture(PAWN,D5,C6,PAWN);
    set_epcapture(&d5c6);
    move b5c5 = to_capture(KING,B5,C5,PAWN);
    move g1c5 = to_capture(BISHOP,G1,C5,PAWN);
    move g8g7 = to_move(ROOK,G8,G7);

    assert(is_in_move_list(d5c6,moves,endp));
    assert(is_in_move_list(b5c5,moves,endp));
    assert(is_in_move_list(g1c5,moves,endp));
    assert(is_in_move_list(g8g7,moves,endp));

    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = g8g7;
    mo_dto.hash_move = d5c6;
    mo_dto.killer1 = 0;
    mo_dto.killer2 = 0;
    mo_dto.next_stage = PVMOVE;

    move *nextp;
    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g8g7);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==d5c6);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g1c5 || clear_score(*nextp)==b5c5);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g1c5 || clear_score(*nextp)==b5c5);
}

void test_winning_caps_before_noncaps() {
    position pos;
    set_pos(&pos,"5b2/p4PPk/1p6/8/K7/1R6/1p6/8 w - -");
    move moves[MAX_MOVES_PER_PLY];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move b3f3 = to_move(ROOK,B3,F3);
    move b3a3 = to_move(ROOK,B3,A3);
    move b3c3 = to_move(ROOK,B3,C3);

    assert(is_in_move_list(b3f3,moves,endp));
    assert(is_in_move_list(b3a3,moves,endp));
    assert(is_in_move_list(b3c3,moves,endp));

    //// capturing promotions
    move g7f8q = to_capture(PAWN,G7,F8,BISHOP);
    set_promopiece(&g7f8q,QUEEN);

    move g7f8r = to_capture(PAWN,G7,F8,BISHOP);
    set_promopiece(&g7f8r,ROOK);

    move g7f8b = to_capture(PAWN,G7,F8,BISHOP);
    set_promopiece(&g7f8b,BISHOP);

    move g7f8n = to_capture(PAWN,G7,F8,BISHOP);
    set_promopiece(&g7f8n,KNIGHT);

    assert(is_in_move_list(g7f8q,moves,endp));
    assert(is_in_move_list(g7f8r,moves,endp));
    assert(is_in_move_list(g7f8b,moves,endp));
    assert(is_in_move_list(g7f8n,moves,endp));

    /// non-capturing promotions
    move g7g8q = to_move(PAWN,G7,G8);
    set_promopiece(&g7g8q,QUEEN);

    move g7g8r = to_move(PAWN,G7,G8);
    set_promopiece(&g7g8r,ROOK);

    move g7g8b = to_move(PAWN,G7,G8);
    set_promopiece(&g7g8b,BISHOP);

    move g7g8n = to_move(PAWN,G7,G8);
    set_promopiece(&g7g8n,KNIGHT);

    assert(is_in_move_list(g7g8q,moves,endp));
    assert(is_in_move_list(g7g8r,moves,endp));
    assert(is_in_move_list(g7g8b,moves,endp));
    assert(is_in_move_list(g7g8n,moves,endp));

    ///
    move b3b6 = to_capture(ROOK,B3,B6,PAWN);
    move b3b2 = to_capture(ROOK,B3,B2,PAWN);

    assert(is_in_move_list(b3b6,moves,endp));
    assert(is_in_move_list(b3b2,moves,endp));

    /////////

    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = b3f3;
    mo_dto.hash_move = b3a3;
    mo_dto.killer1 = b3f3;
    mo_dto.killer2 = b3c3;
    mo_dto.next_stage = PVMOVE;

    move *nextp;
    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==b3f3);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==b3a3);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g7f8q);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g7f8r);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g7f8b);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g7f8n);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g7g8q);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g7g8r);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g7g8b);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==g7g8n);

    ///////////////
    // in MVV/LVA
    // these two moves are interchangeable
    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==b3b2);

    assert(next(&pos,&nextp,&mo_dto));
    assert(clear_score(*nextp)==b3b6);
}

void test_caps_in_order_white() {
    position pos;
    set_pos(&pos,"7k/8/4p3/R2p2Q1/4P3/1B6/8/7K w - - ");

    move moves[50];
    move *endp = gen_legal_moves(moves,&pos,true,false);

    move e4d5 = to_capture(PAWN,E4,D5,PAWN);
    assert(is_in_move_list(e4d5,moves,endp));

    move b3d5 = to_capture(BISHOP,B3,D5,PAWN);
    assert(is_in_move_list(b3d5,moves,endp));

    move a5d5 = to_capture(ROOK,A5,D5,PAWN);
    assert(is_in_move_list(a5d5,moves,endp));

    move g5d5 = to_capture(QUEEN,G5,D5,PAWN);
    assert(is_in_move_list(g5d5,moves,endp));

    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = 0;
    mo_dto.hash_move = 0;
    mo_dto.killer1 = 0;
    mo_dto.killer2 = 0;
    mo_dto.next_stage = PVMOVE;

    move *m;
    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == e4d5);

    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == b3d5);

    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == a5d5);

    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == g5d5);
}

void test_caps_in_order_black() {
    position pos;
    set_pos(&pos,"7k/8/4p3/r2P2q1/4P3/1b6/8/7K b - - ");

    move moves[50];
    move *endp = gen_legal_moves(moves,&pos,true,false);

    move e6d5 = to_capture(PAWN,E6,D5,PAWN);
    assert(is_in_move_list(e6d5,moves,endp));

    move b3d5 = to_capture(BISHOP,B3,D5,PAWN);
    assert(is_in_move_list(b3d5,moves,endp));

    move a5d5 = to_capture(ROOK,A5,D5,PAWN);
    assert(is_in_move_list(a5d5,moves,endp));

    move g5d5 = to_capture(QUEEN,G5,D5,PAWN);
    assert(is_in_move_list(g5d5,moves,endp));

    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = 0;
    mo_dto.hash_move = 0;
    mo_dto.killer1 = 0;
    mo_dto.killer2 = 0;
    mo_dto.next_stage = PVMOVE;

    move *m;
    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == e6d5);

    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == b3d5);

    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == a5d5);

    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == g5d5);
}

void test_ep_ordered_as_capture() {
    position pos;
    set_pos(&pos,"8/3p4/8/4P3/8/8/K6k/8 b - -");

    apply_move(&pos,to_move(PAWN,D7,D5),gundos);

    move moves[50];
    move *endp = gen_legal_moves(moves,&pos,true,false);

    move epcap = to_capture(PAWN,E5,D6,PAWN);
    set_epcapture(&epcap);
    assert(is_in_move_list(epcap,moves,endp));

    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = 0;
    mo_dto.hash_move = 0;
    mo_dto.killer1 = 0;
    mo_dto.killer2 = 0;
    mo_dto.next_stage = PVMOVE;

    move *m;
    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == epcap);
}

void test_killers() {
    position pos;
    reset_pos(&pos);

    move moves[50];
    move *endp = gen_legal_moves(moves,&pos,true,true);

    move h2h3 = to_move(PAWN,H2,H3);
    assert(is_in_move_list(h2h3,moves,endp));

    move g2g4 = to_move(PAWN,G2,G4);
    assert(is_in_move_list(g2g4,moves,endp));

    move_order_dto mo_dto;
    mo_dto.startp = mo_dto.currentp = mo_dto.endp = moves;
    mo_dto.pv_move = 0;
    mo_dto.hash_move = 0;
    mo_dto.killer1 = h2h3;
    mo_dto.killer2 = g2g4;
    mo_dto.next_stage = PVMOVE;

    move *m;
    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == h2h3);
    next(&pos,&m,&mo_dto);
    assert(clear_score(*m) == g2g4);
}

void assert_all_moves_good(position *pos) {
    move moves[200];
    move *endp = gen_legal_moves(&moves[0],pos,true,true);
    for (move* mp=moves;mp<endp;mp++) {
        assert(good_move(pos,*mp)==(*mp != BADMOVE));
    }
}

void test_good_move() {
    position pos;
    reset_pos(&pos);

    // all moves from the initial position
    assert_all_moves_good(&pos);

    // some moves that are not legal from the initial position
    assert(!good_move(&pos,to_move(KNIGHT,G1,E2)));
    assert(!good_move(&pos,to_move(KNIGHT,B1,C4)));

    // some pawn tests
    set_pos(&pos,"k7/P6P/8/4Pp2/1p6/3p4/1PPP4/K7 w - f6 0 1");
    assert_all_moves_good(&pos);
    assert(!good_move(&pos,to_move(PAWN,A7,A8)));
    assert(!good_move(&pos,to_move(PAWN,A7,B8)));
    assert(!good_move(&pos,to_capture(PAWN,A7,B8,PAWN)));
    assert(!good_move(&pos,to_capture(PAWN,A7,A8,KING)));

    flip_board(&pos);
    assert_all_moves_good(&pos);

    set_pos(&pos,"r5k1/6rp/5B2/8/8/7R/7P/7K w - - 0 1");
    assert(!good_move(&pos,to_move(PAWN,H2,H4)));

    // some knight tests
    set_pos(&pos,"k7/8/4p3/6n1/4P3/8/8/K7 b - -");
    assert_all_moves_good(&pos);
    for (int i=0;i<64;i++) {
        move test_move = to_move(KNIGHT,G5,(square_t)i);
        if (pos.piece[i] != NO_PIECE) {
            set_capture(&test_move,(piece_t)pos.piece[i]);
        }
        assert(good_move(&pos,test_move)==(i==F7 || i==H7 || i==E4 || i==F3 || i==H3));
    }

    // some bishop tests
    set_pos(&pos,"k7/8/1Q3q2/8/3b4/8/8/1K6 b - - 0 1");
    assert_all_moves_good(&pos);
    assert(!good_move(&pos,to_capture(BISHOP,D4,F6,QUEEN)));

    // some rook tests
    set_pos(&pos,"k7/8/1Q3q2/8/2PR1p2/8/8/1K6 w - - 0 1");
    assert_all_moves_good(&pos);

    // some queen tests
    set_pos(&pos,"k7/8/1Q3q2/8/2PQ1p2/8/8/1K6 w - - 0 1");
    assert_all_moves_good(&pos);

    // king tests
    set_pos(&pos,"r3kb1r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
    assert(test_pos_consistency(&pos));
    assert_all_moves_good(&pos);
    move bogus = to_move(KING,E8,G8);
    set_castle(&bogus);
    assert(!good_move(&pos,bogus)); // bishop in the path
}

void test_next() {
    test_best_at_top();
    test_place_move_on_top();
    test_pv();
    test_hash_next();
    test_pv_then_hash();
    test_pv_and_hash_same_move();
    test_promotions();
    test_pv_then_hash_then_captures();
    test_winning_caps_before_noncaps();
    test_caps_in_order_white();
    test_caps_in_order_black();
    test_ep_ordered_as_capture();
    test_killers();
    test_good_move();
}
