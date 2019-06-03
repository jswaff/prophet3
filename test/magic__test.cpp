/*
 * magic__test.cpp
 *
 *  Created on: Dec 22, 2016
 *      Author: James
 */

#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"

void test_bishop_moves() {
    position pos;
    set_pos(&pos,"8/1r3k2/8/3bB3/8/8/8/3K4 b - - 0 1");

    uint64 bishop_moves = get_bishop_moves(&pos,E5,pos.black_pieces);
    assert(bishop_moves==0);

    bishop_moves = get_bishop_moves(&pos,D5,pos.black_pieces);
    assert(bishop_moves==(bb_squares[B7] | bb_squares[F7]));
}

void test_rook_moves() {
    position pos;
    set_pos(&pos,"8/1r3k2/8/3bB3/8/8/8/3K4 b - - 0 1");

    uint64 rook_moves = get_rook_moves(&pos,B7,pos.black_pieces);
    assert(rook_moves==bb_squares[F7]);
}


void test_magic() {
    test_bishop_moves();
    test_rook_moves();
}
