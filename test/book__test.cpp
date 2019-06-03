/*
 * book__test.cpp
 *
 *  Created on: Nov 29, 2016
 *      Author: James
 */


#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"

void test_build_book_key() {
    position pos;
    reset_pos(&pos);

    int64 bkey = build_book_key(&pos);
    assert(bkey==8683256505108505948L);

    set_pos(&pos,"8/4Pk1p/6p1/1r6/8/5N2/2B2PPP/b5K1 w - - ");
    bkey = build_book_key(&pos);
    assert(bkey==5404474865777589476L);

    set_pos(&pos,"8/8/3k4/2n1P3/8/8/3rP3/RN2K2R w KQ - 0 1");
    bkey = build_book_key(&pos);
    assert(bkey==-6221215072275532145L);

    set_pos(&pos,"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    bkey = build_book_key(&pos);
    assert(bkey==8410219886346271288L);

    set_pos(&pos,"r3k2r/8/8/8/8/5Q2/8/4K3 b kq - 0 1");
    bkey = build_book_key(&pos);
    assert(bkey==-7818203441519759474);
}

bool test_book() {
    test_build_book_key();
    return true;
}
