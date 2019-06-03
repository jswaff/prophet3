/*
 * utils__test.cpp
 *
 *  Created on: Nov 26, 2011
 *      Author: james
 */


#include <cassert>
#include <cstring>
#include "../src/defs.h"
#include "../src/globals.h"

void test_opposite_player() {
    assert(opposite_player(WHITE)==BLACK);
    assert(opposite_player(BLACK)==WHITE);
}

void test_str_to_sq() {
    assert(str_to_sq("-")==NO_SQUARE);
    assert(str_to_sq("e3")==E3);
    assert(str_to_sq("a8")==A8);
    assert(str_to_sq("C5")==C5);
}

void test_sq_to_str() {
    char str[3];
    sq_to_str(E3,str);
    assert(!strcmp("e3",str));

    sq_to_str(H8,str);
    assert(!strcmp("h8",str));

    sq_to_str(C6,str);
    assert(!strcmp("c6",str));
}

void test_move_to_str() {
    move mv = to_move(PAWN,C7,C5);
    char buffer[6];
    move_to_str(mv,buffer);
    assert(!strcmp("c7c5",buffer));

    mv = to_move(KNIGHT,B8,C6);
    move_to_str(mv,buffer);
    assert(!strcmp("b8c6",buffer));

    mv = to_move(PAWN,C7,C8);
    set_promopiece(&mv,QUEEN);
    move_to_str(mv,buffer);
    assert(!strcmp("c7c8q",buffer));
}

void test_str_to_move() {
    position pos;
    reset_pos(&pos);
    move actual = str_to_move("e2e4",&pos);
    move expected = to_move(PAWN,E2,E4);

    assert(actual==expected);

    apply_move(&pos,actual,gundos);
    actual = str_to_move("g8f6",&pos);
    expected = to_move(KNIGHT,G8,F6);
    assert(actual==expected);
    apply_move(&pos,actual,gundos);

    // illegal move
    actual = str_to_move("b1d2", &pos);
    expected = BADMOVE;
    assert(actual==expected);

    // capture
    apply_move(&pos,to_move(PAWN,D2,D4),gundos);
    actual = str_to_move("f6e4",&pos);
    expected = to_capture(KNIGHT,F6,E4,PAWN);
    assert(actual==expected);
    assert(get_captured_piece(actual)==PAWN);

    // ep
    set_pos(&pos,"4k3/8/8/8/3pP3/8/7p/R3K3 b Q e3");
    actual = str_to_move("d4e3",&pos);
    expected = to_capture(PAWN,D4,E3,PAWN);
    set_epcapture(&expected);
    assert(actual==expected);
    assert(get_captured_piece(actual)==PAWN);
    apply_move(&pos,actual,gundos);

    // castle
    actual = str_to_move("e1c1",&pos);
    expected = to_move(KING,E1,C1);
    set_castle(&expected);
    assert(actual==expected);
    apply_move(&pos,actual,gundos);

    // promotion
    actual = str_to_move("h2h1r",&pos);
    expected = to_move(PAWN,H2,H1);
    set_promopiece(&expected,ROOK);
    assert(actual==expected);

    // some bogus move
    actual = str_to_move("f3f4",&pos);
    expected = BADMOVE;
    assert(actual==expected);

    //
    set_pos(&pos,"rnb1k2r/ppq1n3/2pppp2/6pp/2PP3N/P2BP3/2PB1PPP/R2Q1RK1 w kq - -");
    actual = str_to_move("h4g6",&pos);
    expected = to_move(KNIGHT,H4,G6);
    assert(actual==expected);

    set_pos(&pos,"r2r2k1/1p2p1b1/1pp1b1pp/4P2n/PP3p1p/2P2N2/5PP1/R3KBNR b KQ b3 -");
    actual = str_to_move("e6b3",&pos);
    expected = to_move(BISHOP,E6,B3);
    assert(actual==expected);

}

void test_is_in_move_list() {
    move mv_list[5];
    mv_list[0] = to_move(PAWN,E2,E4);
    mv_list[1] = to_move(PAWN,E2,E3);
    mv_list[2] = to_move(KNIGHT,G1,F3);
    mv_list[3] = to_move(BISHOP,B1,D3);
    mv_list[4] = to_move(PAWN,D4,E5);
    set_capture(&mv_list[4],PAWN);

    assert(is_in_move_list(to_move(PAWN,E2,E4),mv_list,&mv_list[5]));
    assert(!is_in_move_list(to_move(PAWN,D2,D3),mv_list,&mv_list[5]));
}

void test_is_legal_move() {
    position pos;
    reset_pos(&pos);

    move legal = to_move(PAWN,E2,E4);
    assert(is_legal_move(legal,&pos));

    move illegal = to_move(PAWN,E2,E5);
    assert(!is_legal_move(illegal,&pos));
}

void test_is_white_piece() {
    assert(is_white_piece(PAWN));
    assert(!is_white_piece(-PAWN));
    assert(!is_white_piece(NO_PIECE));
}

void test_is_black_piece() {
    assert(is_black_piece(-QUEEN));
    assert(!is_black_piece(KNIGHT));
    assert(!is_black_piece(NO_PIECE));
}

void test_is_not_white_piece() {
    assert(is_not_white_piece(NO_PIECE));
    assert(is_not_white_piece(-KING));
    assert(!is_not_white_piece(BISHOP));
}

void test_is_not_black_piece() {
    assert(is_not_black_piece(NO_PIECE));
    assert(is_not_black_piece(ROOK));
    assert(!is_not_black_piece(-KNIGHT));
}

// game status checks
void test_gs_initial_pos() {
    reset_pos(&gpos);
    assert(get_game_status()==INPROGRESS);
}

void test_gs_checkmate_foolsmate() {
    set_pos(&gpos,"rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq -");
    assert(get_game_status()==CHECKMATED);
}

void test_gs_checkmate_byrnefischer() {
    set_pos(&gpos,"1Q6/5pk1/2p3p1/1p2N2p/1b5P/1bn5/2r3P1/2K5 w - -");
    assert(get_game_status()==CHECKMATED);
}

void test_gs_checkmate_simplemate() {
    set_pos(&gpos,"3k2R1/8/3K4/8/8/8/8/8 b - -");
    assert(get_game_status()==CHECKMATED);
}

void test_gs_stalemate() {
    set_pos(&gpos,"8/8/8/6K1/8/1Q6/p7/k7 b - -");
    assert(get_game_status()==STALEMATED);
}

void test_gs_stalemate_burnpilsbury() {
    set_pos(&gpos,"5k2/5P2/5K2/8/8/8/8/8 b - -");
    assert(get_game_status()==STALEMATED);
}

void test_gs_stalemate_2() {
    set_pos(&gpos,"kb5R/8/1K6/8/8/8/8/8 b - - ");
    assert(get_game_status()==STALEMATED);
}

void test_gs_stalemate_3() {
    set_pos(&gpos,"8/8/8/8/8/2K5/1R6/k7 b - -");
    assert(get_game_status()==STALEMATED);
}

void test_gs_draw50() {
    reset_pos(&gpos);

    gpos.fifty_counter=gpos.move_counter=49;
    assert(get_game_status()==INPROGRESS);

    gpos.fifty_counter=gpos.move_counter=50;
    assert(get_game_status()==INPROGRESS);

    gpos.fifty_counter=gpos.move_counter=51;
    assert(get_game_status()==INPROGRESS);

    gpos.fifty_counter=gpos.move_counter=100;
    assert(get_game_status()==DRAW_BY_50);
}

void test_gs_no_material() {
    set_pos(&gpos,"kb6/8/1K6/8/8/8/8/8 b - - ");
    assert(get_game_status()==DRAW_MATERIAL);
}

void test_gs_no_material_one_pawn() {
    set_pos(&gpos,"4k3/8/8/8/8/8/P7/4K3 w - -");
    assert(get_game_status()!=DRAW_MATERIAL);
}

void test_gs_no_material_one_knight() {
    set_pos(&gpos,"4k3/8/8/8/8/8/n7/4K3 w - -");
    assert(get_game_status()==DRAW_MATERIAL);
}

void test_gs_no_material_one_bishop() {
    set_pos(&gpos,"4k3/8/8/8/8/8/B7/4K3 w - -");
    assert(get_game_status()==DRAW_MATERIAL);
}

void test_gs_no_material_one_rook() {
    set_pos(&gpos,"4k3/8/8/8/8/8/r7/4K3 b - -");
    assert(get_game_status()!=DRAW_MATERIAL);
}

void test_gs_no_material_one_queen() {
    set_pos(&gpos,"4k3/8/8/8/8/8/Q7/4K3 w - -");
    assert(get_game_status()!=DRAW_MATERIAL);
}

void test_gs_no_material_just_kings() {
    set_pos(&gpos,"4k3/8/8/8/8/8/8/4K3 w - -");
    assert(get_game_status()==DRAW_MATERIAL);
}

void test_gs_no_material_two_white_knights() {
    set_pos(&gpos,"4k3/8/8/8/8/8/NN6/4K3 w - -");
    assert(get_game_status()!=DRAW_MATERIAL);
}

void test_gs_no_material_two_opposing_knights() {
    set_pos(&gpos,"4k3/8/8/8/8/8/Nn6/4K3 b - -");
    assert(get_game_status()!=DRAW_MATERIAL);
}

void test_gs_no_material_two_bishops_different_colors() {
    set_pos(&gpos,"4k3/8/8/8/8/8/Bb6/4K3 w - -");
    assert(get_game_status()!=DRAW_MATERIAL);
}

void test_gs_no_material_two_bishops_same_color() {
    set_pos(&gpos,"4k3/8/8/8/8/8/B1b5/4K3 b - -");
    assert(get_game_status()==DRAW_MATERIAL);
}

void test_gs_no_material_bishop_vs_knight() {
    set_pos(&gpos,"4k3/8/8/8/8/8/B1n5/5K2 b - -");
    assert(get_game_status()!=DRAW_MATERIAL);
}

void test_gs_draw_by_rep() {
    reset_pos(&gpos);
    assert(get_game_status()!=DRAW_REP);

    apply_move(&gpos,to_move(PAWN,E2,E4),gundos);
    assert(get_game_status()!=DRAW_REP);

    apply_move(&gpos,to_move(KNIGHT,G8,F6),gundos);
    assert(get_game_status()!=DRAW_REP);

    apply_move(&gpos,to_move(KNIGHT,G1,F3),gundos);
    assert(get_game_status()!=DRAW_REP);

    apply_move(&gpos,to_move(KNIGHT,F6,G8),gundos);
    assert(get_game_status()!=DRAW_REP);

    apply_move(&gpos,to_move(KNIGHT,F3,G1),gundos);
    assert(get_game_status()!=DRAW_REP); // still 1 (first has ep square)

    apply_move(&gpos,to_move(KNIGHT,G8,F6),gundos);
    assert(get_game_status()!=DRAW_REP); // 2

    apply_move(&gpos,to_move(KNIGHT,G1,F3),gundos);
    assert(get_game_status()!=DRAW_REP); // 2

    apply_move(&gpos,to_move(KNIGHT,F6,G8),gundos);
    assert(get_game_status()!=DRAW_REP); // 2

    apply_move(&gpos,to_move(KNIGHT,F3,G1),gundos);
    assert(get_game_status()!=DRAW_REP); // 2

    apply_move(&gpos,to_move(KNIGHT,G8,F6),gundos);
    assert(get_game_status()==DRAW_REP); // 3

    apply_move(&gpos,to_move(PAWN,D2,D4),gundos);
    assert(get_game_status()!=DRAW_REP);
}

void test_utils() {
    test_opposite_player();
    test_str_to_sq();
    test_sq_to_str();
    test_move_to_str();
    test_str_to_move();
    test_is_in_move_list();
    test_is_legal_move();
    test_is_white_piece();
    test_is_black_piece();
    test_is_not_white_piece();
    test_is_not_black_piece();

    test_gs_initial_pos();
    test_gs_checkmate_foolsmate();
    test_gs_checkmate_byrnefischer();
    test_gs_checkmate_simplemate();
    test_gs_stalemate();
    test_gs_stalemate_burnpilsbury();
    test_gs_stalemate_2();
    test_gs_stalemate_3();
    test_gs_draw50();
    test_gs_no_material();
    test_gs_no_material_one_pawn();
    test_gs_no_material_one_knight();
    test_gs_no_material_one_bishop();
    test_gs_no_material_one_rook();
    test_gs_no_material_one_queen();
    test_gs_no_material_just_kings();
    test_gs_no_material_two_white_knights();
    test_gs_no_material_two_opposing_knights();
    test_gs_no_material_two_bishops_different_colors();
    test_gs_no_material_two_bishops_same_color();
    test_gs_no_material_bishop_vs_knight();
    test_gs_draw_by_rep();
}
