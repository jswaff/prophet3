/*
 * attack.cpp
 *
 *  Created on: Dec 17, 2011
 *      Author: james
 */


#include <cassert>
#include <cstdio>
#include "defs.h"
#include "globals.h"

bool attacked_by_knight(position *p,square_t sq,color_t player) {
    assert(p);
    assert(sq >= A8 && sq <= H1);
    assert(player==WHITE || player==BLACK);

    return bb_knight_moves[sq] & (player==WHITE ? p->white_knights : p->black_knights);
}

bool attacked_by_king(position *p,square_t sq,color_t player) {
    assert(p);
    assert(sq >= A8 && sq <= H1);
    assert(player==WHITE || player==BLACK);

    return bb_king_moves[sq] & bb_squares[player==WHITE ? p->white_king : p->black_king];
}

bool attacked_by_rook(position *p,square_t sq,color_t player) {
    return get_rook_moves(p,sq,player==WHITE ? p->white_rooks : p->black_rooks);
}

bool attacked_by_bishop(position *p,square_t sq,color_t player) {
    return get_bishop_moves(p,sq,player==WHITE ? p->white_bishops : p->black_bishops);
}

bool attacked_by_queen(position *p,square_t sq,color_t player) {
    return get_queen_moves(p,sq,player==WHITE ? p->white_queens : p->black_queens);
}


/**
 * Given position <p> , is square <sq> attacked by a pawn belong to <player>?
 */
bool attacked_by_pawn(position *p,square_t sq,color_t player) {
    assert(p);
    assert(sq >= A8 && sq <= H1);
    assert(player==WHITE || player==BLACK);

    if (player==WHITE) {
        if (((p->white_pawns & ~bb_files[FILE_A]) >> 9) & bb_squares[sq]) return true;
        if (((p->white_pawns & ~bb_files[FILE_H]) >> 7) & bb_squares[sq]) return true;
    } else {
        if (((p->black_pawns & ~bb_files[FILE_A]) << 7) & bb_squares[sq]) return true;
        if (((p->black_pawns & ~bb_files[FILE_H]) << 9) & bb_squares[sq]) return true;
    }

    return false;
}

/**
 * Given position <p>, is square <sq> attacked by player <player> ?
 */
bool attacked(position *p,square_t sq,color_t player) {

    if (attacked_by_pawn(p,sq,player)) return true;
    if (attacked_by_knight(p,sq,player)) return true;
    if (attacked_by_king(p,sq,player)) return true;
    if (attacked_by_rook(p,sq,player)) return true;
    if (attacked_by_bishop(p,sq,player)) return true;
    if (attacked_by_queen(p,sq,player)) return true;

    return false;
}

uint64 get_attackers(position *p,square_t sq,color_t player) {
    uint64 attackers = 0;

    attackers = bb_knight_moves[sq] & (player==WHITE ? p->white_knights : p->black_knights);
    attackers |= bb_king_moves[sq] & (player==WHITE ? bb_squares[p->white_king] : bb_squares[p->black_king]);

    if (player==WHITE) {
        attackers |= ((bb_squares[sq] & ~bb_files[FILE_A]) << 7) & p->white_pawns;
        attackers |= ((bb_squares[sq] & ~bb_files[FILE_H]) << 9) & p->white_pawns;
    } else {
        attackers |= ((bb_squares[sq] & ~bb_files[FILE_A]) >> 9) & p->black_pawns;
        attackers |= ((bb_squares[sq] & ~bb_files[FILE_H]) >> 7) & p->black_pawns;
    }

    attackers |= get_bishop_moves(p,sq,player==WHITE ? p->white_bishops : p->black_bishops);
    attackers |= get_rook_moves(p,sq,player==WHITE ? p->white_rooks : p->black_rooks);
    attackers |= get_queen_moves(p,sq,player==WHITE ? p->white_queens : p->black_queens);

    return attackers;
}

