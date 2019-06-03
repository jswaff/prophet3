/*
 * extend.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: James
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "globals.h"
#include "defs.h"

int32 extend(position *pos,move last_move,bool incheck) {

    assert(incheck==in_check(pos,pos->player));

    if (incheck) return 1;
    if (get_promopiece(last_move)) return 1;

    // pawn to 7th
    /*square_t to_sq = get_to_sq(last_move);
    if (pos->player==WHITE) { // black was on move
        if (pos->piece[to_sq]==-PAWN && get_rank(to_sq)==RANK_2) {
            return 1;
        }
    } else {
        if (pos->piece[to_sq]==PAWN && get_rank(to_sq)==RANK_7) {
            return 1;
        }
    }*/

    return 0;
}

