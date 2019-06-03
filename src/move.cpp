/*
 * move.cpp
 *
 *  Created on: Dec 20, 2011
 *      Author: james
 */

#include <cassert>
#include "defs.h"

move to_move(piece_t piece,square_t from,square_t to) {
    move mv;
    to_move(&mv,piece,from,to);
    return mv;
}

void to_move(move *mv,piece_t piece,square_t from,square_t to) {
    *mv = from | (to << 6) | (piece << 12);
}

move to_capture(piece_t piece,square_t from,square_t to,piece_t captured_piece) {
    move mv;
    to_capture(&mv,piece,from,to,captured_piece);
    return mv;
}

void to_capture(move* mv,piece_t piece,square_t from,square_t to,piece_t captured_piece) {
    *mv = from | (to << 6) | (piece << 12);
    set_capture(mv,captured_piece);
}

void set_promopiece(move *mv,piece_t promo) {
    *mv &= ~(7 << 15);
    *mv |= promo << 15;
}

void set_capture(move *mv,piece_t captured_piece) {
    assert(captured_piece != NO_PIECE);
    int32 cp = captured_piece < NO_PIECE ? -captured_piece : captured_piece;
    assert(cp > 0 && cp < 8);
    *mv &= ~(7 << 18);
    *mv |= cp << 18;
}

void set_epcapture(move *mv) {
    set_capture(mv,PAWN);
    *mv |= 1 << 21;
}

void set_castle(move *mv) {
    *mv |= 1 << 22;
}

square_t get_from_sq(move mv) {
    return (square_t)(mv & 63);
}

square_t get_to_sq(move mv) {
    return (square_t)((mv >> 6) & 63);
}

piece_t get_piece(move mv) {
    return (piece_t)((mv >> 12) & 7);
}

piece_t get_promopiece(move mv) {
    return (piece_t)((mv >> 15) & 7);
}

piece_t get_captured_piece(move mv) {
    return (piece_t)((mv >> 18) & 7);
}

bool is_capture(move mv) {
    piece_t cp = (piece_t)((mv >> 18) & 7);
    return cp != NO_PIECE;
}

bool is_epcapture(move mv) {
    return (mv >> 21) & 1;
}

bool is_castle(move mv) {
    return (mv >> 22) & 1;
}

int32 get_move_score(move *m) {
    int32 score = ((*m >> 32) & 0x7FFFFFFF);
    if ((*m >> 63) & 1) {
        return -score;
    } else {
        return score;
    }
}

void set_move_score(move *m,int32 score) {
    // clear high order bits
    *m &= 0xFFFFFFFF;

    // now set score
    if (score >= 0) {
        *m |= ((uint64)score)<<32;
    } else {
        *m |= ((uint64)-score)<<32;
        *m |= ((uint64)1)<<63;
    }
}

move clear_score(move mv) {
    return mv & 0xFFFFFFFF;
}
