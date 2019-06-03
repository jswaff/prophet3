/*
 * evalp.cpp
 *
 *  Created on: Jan 23, 2016
 *      Author: James
 */

#include <cassert>
#include <cstdlib>
#include "defs.h"
#include "eval.h"
#include "globals.h"

uint64 bb_isolated[64];
uint64 bb_passed[64][2];

int32 eval_pawn(position *p,square_t sq,bool wtm) {
    int32 score = wtm ? pawn_pst[sq] : pawn_pst[fliprank[sq]];

    if (is_passed(p,sq,wtm)) {
        score += passed_pawn;
    }
    if (is_doubled(p,sq,wtm)) {
        score += doubled_pawn;
    }
    if (is_isolated(p,sq,wtm)) {
        score += isolated_pawn;
    }

    return score;
}

bool is_doubled(position *pos,square_t pawn_sq,bool wtm) {
    assert( (wtm && pos->piece[pawn_sq]==PAWN) || (!wtm && pos->piece[pawn_sq]==-PAWN));

    uint64 file_mask = bb_files[get_file(pawn_sq)] ^ bb_squares[pawn_sq];

    if (wtm){
        if (file_mask & pos->white_pawns) return true;
    } else {
        if (file_mask & pos->black_pawns) return true;
    }

    return false;
}

bool is_isolated(const position *pos,const square_t pawn_sq,const bool wtm) {
    assert( (wtm && pos->piece[pawn_sq]==PAWN) || (!wtm && pos->piece[pawn_sq]==-PAWN));
    uint64 friends = wtm ? pos->white_pawns : pos->black_pawns;
    return !(bb_isolated[pawn_sq] & friends);
}

bool is_passed(position *pos,square_t pawn_sq,bool wtm) {
    assert( (wtm && pos->piece[pawn_sq]==PAWN) || (!wtm && pos->piece[pawn_sq]==-PAWN));

    uint64 enemies = wtm ? pos->black_pawns : pos->white_pawns;
    return !(bb_passed[pawn_sq][wtm?WHITE:BLACK] & enemies);
}

void init_evalp_bitboards() {

    // isolated pawn masks
    for (int i=0;i<64;i++) {
        bb_isolated[i] = 0;
        file_t f = get_file((square_t)i);

        if (f > FILE_A) {
            bb_isolated[i] |= bb_files[f-1];
        }
        if (f < FILE_H) {
            bb_isolated[i] |= bb_files[f+1];
        }
    }

    // passed pawn masks
    for (int i=0;i<64;i++) {
        file_t f = get_file((square_t)i);

        bb_passed[i][WHITE] = bb_rays[i][NORTH];
        bb_passed[i][BLACK] = bb_rays[i][SOUTH];

        if (f > FILE_A) {
            bb_passed[i][WHITE] |= bb_rays[i-1][NORTH];
            bb_passed[i][BLACK] |= bb_rays[i-1][SOUTH];
        }
        if (f < FILE_H) {
            bb_passed[i][WHITE] |= bb_rays[i+1][NORTH];
            bb_passed[i][BLACK] |= bb_rays[i+1][SOUTH];
        }
    }
}
