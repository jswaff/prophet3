/*
 * score.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: James
 */

#include <cassert>
#include "defs.h"
#include "globals.h"

int32 mvvlva_promo(move mv) {
    piece_t promo = get_promopiece(mv);
    assert(promo==QUEEN || promo==ROOK || promo==BISHOP || promo==KNIGHT);

    return 10000 + promo;
}

int32 mvvlva_capture(position *pos,move mv) {
    piece_t captured;
    if (is_epcapture(mv)) {
        captured = PAWN;
    } else {
        captured = (piece_t)pos->piece[get_to_sq(mv)];
    }
    assert(captured != NO_PIECE);
    int32 captured_val = captured < NO_PIECE ? -captured : captured;

    piece_t mover = (piece_t)pos->piece[get_from_sq(mv)];
    assert(mover != NO_PIECE);
    int32 mover_val = mover < NO_PIECE ? -mover : mover;

    return 1000 + (captured_val * 10) - mover_val;
}

/**
 * Score the move.  Promotions are scored highest of all, with capturing promotions
 * being higher than non-capturing promotions.  Secondary to capture vs non-capture
 * is the piece being promoted to.  Queen promotions are higher, followed by rook,
 * bishop, then knight.
 *
 * Captures are scored next using the MVV/LVA algorithm.  This is not a very accurate
 * algorithm, but it's fast and cheap.  The idea is to score moves that capture bigger
 * pieces higher. If two moves capture the same piece, the move with the smallest capturer
 * is scored higher.
 * PxQ, NxQ, BxQ, RxQ, QxQ, KxQ, PxR, BxR, NxR, RxR, QxR, KxR ...
 */
int32 mvvlva(position *pos,move mv) {
    int32 score = 0;

    if (get_promopiece(mv)!=NO_PIECE) {
        score = mvvlva_promo(mv);
    }
    if (is_capture(mv)) {
        score += mvvlva_capture(pos,mv);
    }

    return score;
}
