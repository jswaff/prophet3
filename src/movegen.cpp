/*
 * movegen.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: james
 */

#include <cassert>
#include <cstring>
#include "defs.h"
#include "globals.h"

uint64 get_target_squares(position* p,bool caps,bool noncaps);

/**
 * Generate legal moves for position <p>, beginning at move pointer <m>.
 * The returned move pointer is one greater than the last move. [m,endp)
 *
 * NOTE: the list will contain '0' at places that illegal moves were purged.
 */
move* gen_legal_moves(move *m,position *p,bool caps,bool noncaps) {
    move* endp = gen_moves(m,p,caps,noncaps);

    undo undos[p->move_counter + (endp-m)];
    for (move* mp=m;mp<endp;mp++) {
        apply_move(p,*mp,undos);
        if (in_check(p,opposite_player(p->player))) {
            // mark as 'invalid'
            *mp = 0;
        }
        undo_last_move(p,undos);
    }

    return endp;
}

/**
 * Generate pseudo-legal moves for position <p>, beginning at move pointer <m>.
 * The returned move pointer is one greater than the last move. [m, endp)
 */
move* gen_moves(move *m,position *p,bool caps,bool noncaps) {

    m = gen_pawn_moves(m,p,caps,noncaps);
    m = gen_knight_moves(m,p,caps,noncaps);
    m = gen_bishop_moves(m,p,caps,noncaps);
    m = gen_rook_moves(m,p,caps,noncaps);
    m = gen_queen_moves(m,p,caps,noncaps);
    m = gen_king_moves(m,p,caps,noncaps);

    return m;
}


move* add_move(move* m,position* p,piece_t piece,square_t from,square_t to) {
    piece_t captured_piece = (piece_t)p->piece[to];
    assert( (p->player==WHITE && is_not_white_piece(captured_piece))
        || (p->player==BLACK && is_not_black_piece(captured_piece)));

    to_move(m,piece,from,to);
    if (captured_piece != NO_PIECE) {
        set_capture(m,captured_piece);
    }
    ++m;

    return m;
}

move* add_castle(move* m,square_t from,square_t to) {
    to_move(m,KING,from,to);
    set_castle(m);
    ++m;
    return m;
}

move* add_promotion(move* m,square_t from,square_t to,piece_t promopiece,piece_t captured_piece) {
    to_move(m,PAWN,from,to);
    if (captured_piece != NO_PIECE) {
        set_capture(m,captured_piece);
    }
    set_promopiece(m,promopiece);
    ++m;
    return m;
}

/**
 * Add the pawn move to the list.  If the move is a capture then the capture
 * flag is set.  If it is a promotion then four moves are added.
 */
move* add_pawn_move(move* m,square_t from,square_t to,piece_t captured_piece,bool epcapture) {
    rank_t r = get_rank(to);
    if (r==RANK_8 || r==RANK_1) {
        m = add_promotion(m,from,to,QUEEN,captured_piece);
        m = add_promotion(m,from,to,KNIGHT,captured_piece);
        m = add_promotion(m,from,to,ROOK,captured_piece);
        m = add_promotion(m,from,to,BISHOP,captured_piece);
    } else {
        to_move(m,PAWN,from,to);
        if (epcapture) {
            set_epcapture(m);
        } else if (captured_piece != NO_PIECE) {
            set_capture(m,captured_piece);
        }
        ++m;
    }

    return m;
}

move* gen_knight_moves(move* m,position* p,bool caps,bool noncaps) {
    assert(caps || noncaps);
    uint64 pmap = p->player==WHITE ? p->white_knights : p->black_knights;
    while (pmap) {
        square_t sq = (square_t)get_msb(pmap);
        m = gen_knight_moves(m,p,sq,caps,noncaps);
        pmap ^= bb_squares[sq];
    }

    return m;
}

move* gen_knight_moves(move* m,position* p,square_t from,bool caps,bool noncaps) {
    assert(m);
    assert(p);
    assert(from >= A8 && from <= H1);

    uint64 mv_map = bb_knight_moves[from] & get_target_squares(p,caps,noncaps);
    while (mv_map) {
        square_t sq = (square_t)get_lsb(mv_map);
        m=add_move(m,p,KNIGHT,from,sq);
        mv_map ^= bb_squares[sq];
    }

    return m;
}

move* gen_bishop_moves(move* m,position* p,bool caps,bool noncaps) {

    uint64 pmap = p->player==WHITE ? p->white_bishops : p->black_bishops;
    while (pmap) {
        square_t sq = (square_t)get_msb(pmap);
        m = gen_bishop_moves(m,p,sq,caps,noncaps);
        pmap ^= bb_squares[sq];
    }

    return m;
}

move* gen_bishop_moves(move* m,position* p,square_t from,bool caps,bool noncaps) {
    assert(m);
    assert(p);
    assert(from >= A8 && from <= H1);

    uint64 bishop_moves = get_bishop_moves(p,from,get_target_squares(p,caps,noncaps));

    while (bishop_moves) {
        square_t sq = (square_t)get_msb(bishop_moves);
        m = add_move(m,p,BISHOP,from,sq);
        bishop_moves ^= bb_squares[sq];
    }

    return m;
}

move* gen_rook_moves(move* m,position* p,bool caps,bool noncaps) {

    uint64 pmap = p->player==WHITE ? p->white_rooks : p->black_rooks;
    while (pmap) {
        square_t sq = (square_t)get_msb(pmap);
        m = gen_rook_moves(m,p,sq,caps,noncaps);
        pmap ^= bb_squares[sq];
    }

    return m;
}

move* gen_rook_moves(move* m,position* p,square_t from,bool caps,bool noncaps) {
    assert(m);
    assert(p);
    assert(from >= A8 && from <= H1);

    uint64 rook_moves = get_rook_moves(p,from,get_target_squares(p,caps,noncaps));

    while (rook_moves) {
        square_t sq = (square_t)get_msb(rook_moves);
        m = add_move(m,p,ROOK,from,sq);
        rook_moves ^= bb_squares[sq];
    }

    return m;
}

move* gen_queen_moves(move* m,position* p,bool caps,bool noncaps) {

    uint64 pmap = p->player==WHITE ? p->white_queens : p->black_queens;
    while (pmap) {
        square_t sq = (square_t)get_msb(pmap);
        m = gen_queen_moves(m,p,sq,caps,noncaps);
        pmap ^= bb_squares[sq];
    }

    return m;
}

move* gen_queen_moves(move* m,position* p,square_t from,bool caps,bool noncaps) {
    assert(m);
    assert(p);
    assert(from >= A8 && from <= H1);

    uint64 queen_moves = get_queen_moves(p,from,get_target_squares(p,caps,noncaps));

    while (queen_moves) {
        square_t sq = (square_t)get_msb(queen_moves);
        m = add_move(m,p,QUEEN,from,sq);
        queen_moves ^= bb_squares[sq];
    }

    return m;
}

move* gen_king_moves(move* m,position* p,bool caps,bool noncaps) {
    assert(m);
    assert(p);

    square_t from = p->player==WHITE ? p->white_king : p->black_king;
    uint64 mv_map = bb_king_moves[from] & get_target_squares(p,caps,noncaps);
    while (mv_map) {
        square_t sq = (square_t)get_lsb(mv_map);
        m=add_move(m,p,KING,from,sq);
        mv_map ^= bb_squares[sq];
    }

    // castling moves
    if (noncaps) {
        if (p->player==WHITE && from==E1 && !attacked(p,E1,BLACK)) {
            if (can_castle_wq(p) && is_empty_sq(p,D1) && is_empty_sq(p,C1) && is_empty_sq(p,B1)
                    && !attacked(p,D1,BLACK)) {
                m=add_castle(m,E1,C1);
            }
            if (can_castle_wk(p) && is_empty_sq(p,F1) && is_empty_sq(p,G1)
                    && !attacked(p,F1,BLACK)) {
                m=add_castle(m,E1,G1);
            }
        } else if (p->player==BLACK && from==E8 && !attacked(p,E8,WHITE)) {
            if (can_castle_bq(p) && is_empty_sq(p,D8) && is_empty_sq(p,C8) && is_empty_sq(p,B8)
                    && !attacked(p,D8,WHITE)) {
                m=add_castle(m,E8,C8);
            }
            if (can_castle_bk(p) && is_empty_sq(p,F8) && is_empty_sq(p,G8)
                    && !attacked(p,F8,WHITE)) {
                m=add_castle(m,E8,G8);
            }
        }
    }

    return m;
}

move* gen_pawn_moves(move* m,position* p,bool caps,bool noncaps) {

    uint64 all_pieces = p->white_pieces | p->black_pieces;
    uint64 pmap;

    // captures
    if (p->player==WHITE) {
        if (caps) {
            uint64 targets = p->black_pieces;
            if (p->ep_sq != NO_SQUARE) targets |= bb_squares[p->ep_sq];

            // attacks west
            pmap = ((p->white_pawns & ~bb_files[FILE_A]) >> 9) & targets;
            while (pmap) {
                square_t sq = (square_t)get_msb(pmap);
                piece_t captured = sq==p->ep_sq ? PAWN : (piece_t)p->piece[sq];
                m = add_pawn_move(m,southeast(sq),sq,captured,sq==p->ep_sq);
                pmap ^= bb_squares[sq];
            }

            // attacks east
            pmap = ((p->white_pawns & ~bb_files[FILE_H]) >> 7) & targets;
            while (pmap) {
                square_t sq = (square_t)get_msb(pmap);
                piece_t captured = sq==p->ep_sq ? PAWN : (piece_t)p->piece[sq];
                m = add_pawn_move(m,southwest(sq),sq,captured,sq==p->ep_sq);
                pmap ^= bb_squares[sq];
            }

            // push promotions
            pmap = ((p->white_pawns & bb_ranks[RANK_7]) >> 8) & ~all_pieces;
            while (pmap) {
                square_t sq = (square_t)get_msb(pmap);
                m = add_pawn_move(m,south(sq),sq,NO_PIECE,false);
                pmap ^= bb_squares[sq];
            }
        }

        // pawn pushes less promotions
        if (noncaps) {
            pmap = ((p->white_pawns & ~bb_ranks[RANK_7]) >> 8) & ~all_pieces;
            while (pmap) {
                square_t sq = (square_t)get_msb(pmap);
                m = add_pawn_move(m,south(sq),sq,NO_PIECE,false);
                if (get_rank(sq)==RANK_3 && p->piece[north(sq)]==NO_PIECE) {
                    m = add_pawn_move(m,south(sq),north(sq),NO_PIECE,false);
                }
                pmap ^= bb_squares[sq];
            }
        }
    } else {
        if (caps) {
            uint64 targets = p->white_pieces;
            if (p->ep_sq != NO_SQUARE) targets |= bb_squares[p->ep_sq];

            // attacks west
            pmap = ((p->black_pawns & ~bb_files[FILE_A]) << 7) & targets;
            while (pmap) {
                square_t sq = (square_t)get_lsb(pmap);
                piece_t captured = sq==p->ep_sq ? PAWN : (piece_t)p->piece[sq];
                m = add_pawn_move(m,northeast(sq),sq,captured,sq==p->ep_sq);
                pmap ^= bb_squares[sq];
            }

            // attacks west
            pmap = ((p->black_pawns & ~bb_files[FILE_H]) << 9) & targets;
            while (pmap) {
                square_t sq = (square_t)get_lsb(pmap);
                piece_t captured = sq==p->ep_sq ? PAWN : (piece_t)p->piece[sq];
                m = add_pawn_move(m,northwest(sq),sq,captured,sq==p->ep_sq);
                pmap ^= bb_squares[sq];
            }

            // push promotions
            pmap = ((p->black_pawns & bb_ranks[RANK_2]) << 8) & ~all_pieces;
            while (pmap) {
                square_t sq = (square_t)get_lsb(pmap);
                m = add_pawn_move(m,north(sq),sq,NO_PIECE,false);
                pmap ^= bb_squares[sq];
            }
        }

        // pawn pushes less promotions
        if (noncaps) {
            pmap = ((p->black_pawns & ~bb_ranks[RANK_2]) << 8) & ~all_pieces;
            while (pmap) {
                square_t sq = (square_t)get_lsb(pmap);
                m = add_pawn_move(m,north(sq),sq,NO_PIECE,false);
                if (get_rank(sq)==RANK_6 && p->piece[south(sq)]==NO_PIECE) {
                    m = add_pawn_move(m,north(sq),south(sq),NO_PIECE,false);
                }
                pmap ^= bb_squares[sq];
            }
        }
    }

    return m;
}

uint64 get_target_squares(position* p,bool caps,bool noncaps) {
    uint64 targets = 0;

    if (caps) {
        targets = (p->player==WHITE ? p->black_pieces : p->white_pieces);
    }

    if (noncaps) {
        targets |= ~(p->white_pieces | p->black_pieces);
    }

    return targets;
}
