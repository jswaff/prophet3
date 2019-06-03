/*
 * draw.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: James
 */

#include <cassert>
#include "defs.h"
#include "globals.h"

/**
 * Is the current position draw due to lack of mating material?
 *
 * From the xboard documentation:
 * Note that (in accordance with FIDE rules) only KK, KNK, KBK and KBKB with all bishops on
 * the same color can be claimed as draws on the basis of insufficient mating material. The
 * end-games KNNK, KBKN, KNKN and KBKB with unlike bishops do have mate positions, and cannot
 * be claimed. Complex draws based on locked Pawn chains will not be recognized as draws by
 * most interfaces, so do not claim in such positions, but just offer a draw or play on.
 *
 */
bool is_lack_of_mating_material(position *pos) {
    assert(pos);

    int num_knights=0,num_white_sq_bishops=0,num_black_sq_bishops=0;

    for (int sq=0;sq<64;sq++) {
        switch(pos->piece[sq]) {
            // any position with a pawn, rook, or queen is not drawn by lack of mating material
            case PAWN:
            case -PAWN:
            case ROOK:
            case -ROOK:
            case QUEEN:
            case -QUEEN:
                return false;

            // a position with more than one knight is not drawn by lack of mating material
            case KNIGHT:
            case -KNIGHT:
                num_knights++;
                if (num_knights>1) return false;
                if (num_white_sq_bishops>0 || num_black_sq_bishops>0) return false;
                break;

            // a position with bishops on opposite colors is not drawn
            case BISHOP:
            case -BISHOP:
                if (is_light_sq((square_t)sq)) {
                    num_white_sq_bishops++;
                } else {
                    num_black_sq_bishops++;
                }
                if (num_white_sq_bishops>0 && num_black_sq_bishops>0) return false;
                if (num_knights>0) return false;
                break;
        }
    }

    return true;
}

bool is_draw_by_rep(position *pos,undo *undo_stack) {
    assert(pos);
    assert(pos->move_counter >= 0 && pos->move_counter < MAX_MOVES_PER_GAME);
    assert(pos->fifty_counter >= 0 && pos->fifty_counter <= pos->move_counter);
    assert(pos->hash_key==build_hash_key(pos));
    assert(undo_stack);

    uint32 reps = 0;
    for (uint32 i=(pos->move_counter-pos->fifty_counter);i<pos->move_counter;i++) {
        if ((undo_stack+i)->hash_key==pos->hash_key) {
            reps++;
        }
    }

    return reps >= 2;
}

bool is_draw_by_50(position *pos) {
    assert(pos);
    return pos->fifty_counter >= 100;
}

