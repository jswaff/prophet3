/*
 * search__test.cpp
 *
 *  Created on: Dec 27, 2011
 *      Author: james
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "../src/globals.h"
#include "../src/defs.h"
#include "../src/eval.h"

move move_stack[MOVE_STACK_SIZE];

void start_timer(int milliseconds) {
    set_start_time(milli_timer());
    set_stop_time(get_start_time() + milliseconds);
}

void test_matein1() {
    position pos;
    set_pos(&pos,"4k3/8/3Q4/2B5/8/8/1K6/8 w - -");
    clear_hash_table(&htbl);

    move_line pv; pv.n=0;
    search_stats stats; stats.nodes=0; stats.last_pv.n=0;
    start_timer(10000);
    int32 score=search(&pos,&pv,-INF,INF,2,&stats,move_stack,gundos,false);
    assert(score==CHECKMATE-1);
}

void test_matein1b() {
    position pos;
    set_pos(&pos,"4K3/8/8/3n2q1/8/8/3k4/8 b - -");
    clear_hash_table(&htbl);

    move_line pv; pv.n=0;
    search_stats stats; stats.nodes=0; stats.last_pv.n=0;
    start_timer(10000);
    int32 score=search(&pos,&pv,-INF,INF,2,&stats,move_stack,gundos,false);
    assert(score==CHECKMATE-1);
}

void test_matein2() {
    position pos;
    set_pos(&pos,"r1bq2r1/b4pk1/p1pp1p2/1p2pP2/1P2P1PB/3P4/1PPQ2P1/R3K2R w - -");
    clear_hash_table(&htbl);

    move_line pv; pv.n=0;
    search_stats stats; stats.nodes=0; stats.last_pv.n=0;
    start_timer(10000);
    int32 score=search(&pos,&pv,-INF,INF,4,&stats,move_stack,gundos,false);
    assert(score==CHECKMATE-3);
}

void test_matein3() {
    position pos;
    set_pos(&pos,"r5rk/5p1p/5R2/4B3/8/8/7P/7K w - -");
    clear_hash_table(&htbl);

    move_line pv; pv.n=0;
    search_stats stats; stats.nodes=0; stats.last_pv.n=0;
    start_timer(10000);
    int32 score=search(&pos,&pv,-INF,INF,6,&stats,move_stack,gundos,false);
    assert(score==CHECKMATE-5);
}

void test_stalemate() {
    position pos;
    set_pos(&pos,"8/6p1/5p2/5k1K/7P/8/8/8 w - -");
    clear_hash_table(&htbl);

    move_line pv; pv.n=0;
    search_stats stats; stats.nodes=0; stats.last_pv.n=0;
    start_timer(10000);
    int32 score=search(&pos,&pv,-INF,INF,1,&stats,move_stack,gundos,false);
    assert(score==DRAWSCORE);
    assert(pv.n==0);
}

/*
 * The search is never aborted in the root, so a depth=1 search will always make
 * every move and evaluate it, ensuring we have something to play.
 *
 * Starting a search with depth > 1 does not give the same guarantee, as the
 * search could be aborted in the recursive helper.
 */

void test_abort_search() {
    position pos;
    reset_pos(&pos);
    clear_hash_table(&htbl);

    move_line pv; pv.n=0;
    search_stats stats; stats.nodes=0; stats.last_pv.n=0;
    start_timer(10000);

    set_abort_search(true);
    search(&pos,&pv,-INF,INF,1,&stats,move_stack,gundos,false);
    //assert(stats.nodes==21); // 1 + 20


    pv.n=0;
    stats.nodes=0;
    search(&pos,&pv,-INF,INF,3,&stats,move_stack,gundos,false);
    assert(stats.nodes==4); // 1 + 3 (down the left side of the tree)

    set_abort_search(false);
}

void test_search_last_pv_first() {
    position pos;
    reset_pos(&pos);
    clear_hash_table(&htbl);

    // create an artificial PV and ensure the search examines it first
    search_stats stats;
    stats.last_pv.n=4;
    move c2c4 = stats.last_pv.mv[0] = to_move(PAWN,C2,C4);
    move b7b5 = stats.last_pv.mv[1] = to_move(PAWN,B7,B5);
    move c4b5 = to_move(PAWN,C4,B5);
    set_capture(&c4b5,PAWN);
    stats.last_pv.mv[2] = c4b5;
    move g8h6 = stats.last_pv.mv[3] = to_move(KNIGHT,G8,H6);

    char pv_text[100];
    line_to_str(&stats.last_pv,pv_text);
    //print("last pv: %s\n",pv_text);

    stats.nodes=0;
    start_timer(10000);
    stats.first_line_searched.n=0;
    for (int i=0;i<MAX_PLY;i++) {
        stats.first_line_searched.mv[i] = BADMOVE;
    }
    move_line pv; pv.n=0;
    search(&pos,&pv,-INF,INF,5,&stats,move_stack,gundos,false);

    line_to_str(&stats.first_line_searched,pv_text);
    //print("first line length: %d\n",stats.first_line_searched.n);
    //print("first line: %s\n",pv_text);

    assert(stats.first_line_searched.mv[0]==c2c4);
    assert(stats.first_line_searched.mv[1]==b7b5);
    assert(stats.first_line_searched.mv[2]==c4b5);
    assert(stats.first_line_searched.mv[3]==g8h6);
}


void test_hash_table() {
    position pos;
    reset_pos(&pos);
    clear_hash_table(&htbl);

    position save_pos;
    memcpy(&save_pos,&pos,sizeof(position));

    search_stats stats; stats.nodes=0; stats.last_pv.n=0;
    start_timer(10000);
    stats.first_line_searched.n=0;
    set_abort_search(false);

    // make a (ridiculous) entry up.  the program would never play 1. a4, UNLESS of course it would be a piece up!
    // make 1. ... a5 a rook up for white, everything else a queen up, and verify PV is 1. a4 a5
    move a2a4 = to_move(PAWN,A2,A4);
    apply_move(&pos,a2a4,gundos);
    position save_pos2;
    memcpy(&save_pos2,&pos,sizeof(position));

    move a7a5 = to_move(PAWN,A7,A5);

    move moves[50];
    move *endp = gen_legal_moves(moves,&pos,true,true);
    assert(is_in_move_list(a7a5,moves,endp));

    for (move *mp=moves;mp<endp;mp++) {
        apply_move(&pos,*mp,gundos);

        int32 score = 900;
        if (*mp == a7a5) {
            score = 500;
        }
        store_hash_entry(&htbl,pos.hash_key,build_hash_val(EXACT_SCORE,1,score,*mp));

        memcpy(&pos,&save_pos2,sizeof(position));
    }
    memcpy(&pos,&save_pos,sizeof(position));

    move_line pv; pv.n=0;
    search(&pos,&pv,-INF,INF,3,&stats,move_stack,gundos,false);
    assert(pv.n==2);
    assert(a2a4==pv.mv[0]);
    assert(a7a5==pv.mv[1]);
}

void test_draw_50() {
    position pos;
    // minus one rook, to ensure not draw by eval
    set_pos(&pos,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    clear_hash_table(&htbl);

    move_line pv; pv.n=0;
    search_stats stats; stats.nodes=0; stats.last_pv.n=0;
    start_timer(10000);

    set_abort_search(false);
    int32 score = search(&pos,&pv,-INF,INF,2,&stats,move_stack,gundos,false);
    assert(score != DRAWSCORE);

    // up to 99 (half) moves ... the extra comes from the root search
    pos.fifty_counter = pos.move_counter= 98;
    start_timer(10000);
    clear_hash_table(&htbl);
    score = search(&pos,&pv,-INF,INF,2,&stats,move_stack,gundos,false);
    assert(score != DRAWSCORE);

    // trigger 50 move rule
    pos.fifty_counter = pos.move_counter = 99;
    start_timer(10000);
    clear_hash_table(&htbl);
    score = search(&pos,&pv,-INF,INF,2,&stats,move_stack,gundos,false);
    assert(score == DRAWSCORE);
}

void test_qsearch_does_not_expand_initial_position() {
    position pos;
    reset_pos(&pos);

    clear_hash_table(&htbl);

    search_stats stats; stats.nodes=0; stats.qnodes=0; stats.last_pv.n=0;
    start_timer(10000);
    move moves[50];

    set_abort_search(false);
    qsearch(&pos,-INF,INF,0,0,false,&stats,moves,gundos);

    assert(stats.nodes==0);
    assert(stats.qnodes==0);
}

void test_qsearch_standpat_raises_alpha() {
    position pos;
    reset_pos(&pos);

    clear_hash_table(&htbl);
    search_stats stats; stats.nodes=0; stats.qnodes=0; stats.last_pv.n=0;
    start_timer(10000);

    int32 score = eval(&pos,false);

    move moves[50];

    set_abort_search(false);
    int32 qscore = qsearch(&pos,-INF,INF,0,0,false,&stats,moves,gundos);
    assert(score==qscore);
}

void test_qsearch_standpat_does_not_raise_alpha() {
    position pos;
    reset_pos(&pos);

    clear_hash_table(&htbl);

    search_stats stats; stats.nodes=0; stats.qnodes=0; stats.last_pv.n=0;
    start_timer(10000);
    move moves[50];

    set_abort_search(false);
    int32 qscore = qsearch(&pos,queen_val,INF,0,0,false,&stats,moves,gundos);
    assert(queen_val==qscore);
}

void test_qsearch_expands_just_captures() {
    position pos;
    set_pos(&pos,"7k/8/8/3b4/8/8/6P1/K7 b - -");

    clear_hash_table(&htbl);

    search_stats stats; stats.nodes=0; stats.qnodes=0; stats.last_pv.n=0;
    start_timer(10000);
    move moves[50];

    set_abort_search(false);
    qsearch(&pos,-INF,INF,0,0,false,&stats,moves,gundos);

    assert(stats.nodes==0);
    assert(stats.qnodes==1);
}

void test_qsearch_does_expand_promotions() {
    position pos;
    set_pos(&pos,"8/P6k/8/8/8/8/7K/8 w - -");

    clear_hash_table(&htbl);

    search_stats stats; stats.nodes=0; stats.qnodes=0; stats.last_pv.n=0;
    start_timer(10000);
    move moves[50];

    set_abort_search(false);
    qsearch(&pos,-INF,INF,0,0,false,&stats,moves,gundos);

    assert(stats.nodes==0);
    assert(stats.qnodes==4);
}

void test_qsearch_beta_cutoff() {
    position pos;

    // this position would be very good for white if searched
    set_pos(&pos,"8/P6k/8/8/8/8/7K/8 w - -");
    search_stats stats; stats.nodes=0; stats.qnodes=0; stats.last_pv.n=0;
    start_timer(10000);
    move moves[50];

    int32 score = eval(&pos,false);
    assert(abs(score) < queen_val);

    set_abort_search(false);
    qsearch(&pos,-INF,-queen_val,0,0,false,&stats,moves,gundos);

    assert(stats.nodes==0);
    assert(stats.qnodes==0);
}

void test_search() {
    test_matein1();
    test_matein1b();
    test_matein2();
    test_matein3();
    test_stalemate();

    // this is just to test that the draw code in the search
    // is fired.  testing other draw rules in the context
    // of the search would be pretty difficult to set up
    // but they are tested in draw__test.cpp
    test_draw_50();

    test_abort_search();
    test_search_last_pv_first();
    test_hash_table();

    test_qsearch_does_not_expand_initial_position();
    test_qsearch_standpat_raises_alpha();
    test_qsearch_standpat_does_not_raise_alpha();
    test_qsearch_expands_just_captures();
    test_qsearch_does_expand_promotions();
    test_qsearch_beta_cutoff();
}
