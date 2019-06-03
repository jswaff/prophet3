/*
 * evalks.cpp
 *
 *  Created on: Jan 22, 2016
 *      Author: James
 */


#include "defs.h"
#include "eval.h"
#include "globals.h"

int32 eval_king_safety(position *p,bool wtm) {
    int32 score = 0;

    if (wtm) {
        // what side are we on?
        if (get_file(p->white_king) > FILE_E) {
            // check that pawns on f,g,h are not too far away
            if (p->piece[F2]!=PAWN) {
                if (p->piece[F3]==PAWN) {
                    score += king_safety_pawn_one_away;
                } else if (p->piece[F4]==PAWN) {
                    score += king_safety_pawn_two_away;
                } else {
                    score += king_safety_pawn_far_away;
                }
            }
            if (p->piece[G2]!=PAWN) {
                if (p->piece[G3]==PAWN) {
                    score += king_safety_pawn_one_away;
                } else if (p->piece[G4]==PAWN) {
                    score += king_safety_pawn_two_away;
                } else {
                    score += king_safety_pawn_far_away;
                }
            }
            if (p->piece[H2]!=PAWN) {
                if (p->piece[H3]==PAWN) {
                    score += king_safety_pawn_one_away/2;
                } else if (p->piece[H4]==PAWN) {
                    score += king_safety_pawn_two_away/2;
                } else {
                    score += king_safety_pawn_far_away/2;
                }
            }
        } else if (get_file(p->white_king) < FILE_D) {
            if (p->piece[C2]!=PAWN) {
                if (p->piece[C3]==PAWN) {
                    score += king_safety_pawn_one_away;
                } else if (p->piece[C4]==PAWN) {
                    score += king_safety_pawn_two_away;
                } else {
                    score += king_safety_pawn_far_away;
                }
            }
            if (p->piece[B2]!=PAWN) {
                if (p->piece[B3]==PAWN) {
                    score += king_safety_pawn_one_away;
                } else if (p->piece[B4]==PAWN) {
                    score += king_safety_pawn_two_away;
                } else {
                    score += king_safety_pawn_far_away;
                }
            }
            if (p->piece[A2]!=PAWN) {
                if (p->piece[A3]==PAWN) {
                    score += king_safety_pawn_one_away/2;
                } else if (p->piece[A4]==PAWN) {
                    score += king_safety_pawn_two_away/2;
                } else {
                    score += king_safety_pawn_far_away/2;
                }
            }
        } else {
            // check if open file
            if (! ((p->white_pawns | p->black_pawns) & bb_files[get_file(p->white_king)])) {
                score += king_safety_middle_open_file;
            }
        }
    } else {
        if (get_file(p->black_king) > FILE_E) {
            // check that pawns on f,g,h are not too far away
            if (p->piece[F7]!=-PAWN) {
                if (p->piece[F6]==-PAWN) {
                    score += king_safety_pawn_one_away;
                } else if (p->piece[F5]==-PAWN) {
                    score += king_safety_pawn_two_away;
                } else {
                    score += king_safety_pawn_far_away;
                }
            }
            if (p->piece[G7]!=-PAWN) {
                if (p->piece[G6]==-PAWN) {
                    score += king_safety_pawn_one_away;
                } else if (p->piece[G5]==-PAWN) {
                    score += king_safety_pawn_two_away;
                } else {
                    score += king_safety_pawn_far_away;
                }
            }
            if (p->piece[H7]!=-PAWN) {
                if (p->piece[H6]==-PAWN) {
                    score += king_safety_pawn_one_away/2;
                } else if (p->piece[H5]==-PAWN) {
                    score += king_safety_pawn_two_away/2;
                } else {
                    score += king_safety_pawn_far_away/2;
                }
            }
        } else if (get_file(p->black_king) < FILE_D) {
            if (p->piece[C7]!=-PAWN) {
                if (p->piece[C6]==-PAWN) {
                    score += king_safety_pawn_one_away;
                } else if (p->piece[C5]==-PAWN) {
                    score += king_safety_pawn_two_away;
                } else {
                    score += king_safety_pawn_far_away;
                }
            }
            if (p->piece[B7]!=-PAWN) {
                if (p->piece[B6]==-PAWN) {
                    score += king_safety_pawn_one_away;
                } else if (p->piece[B5]==-PAWN) {
                    score += king_safety_pawn_two_away;
                } else {
                    score += king_safety_pawn_far_away;
                }
            }
            if (p->piece[A7]!=-PAWN) {
                if (p->piece[A6]==-PAWN) {
                    score += king_safety_pawn_one_away/2;
                } else if (p->piece[A5]==-PAWN) {
                    score += king_safety_pawn_two_away/2;
                } else {
                    score += king_safety_pawn_far_away/2;
                }
            }
        } else {
            if (! ((p->white_pawns | p->black_pawns) & bb_files[get_file(p->black_king)])) {
                score += king_safety_middle_open_file;
            }
        }

    }

    return score;
}
