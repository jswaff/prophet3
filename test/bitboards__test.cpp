/*
 * bitboards__test.cpp
 *
 *  Created on: Apr 20, 2016
 *      Author: James
 */

#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"

void test_get_num_bits() {
    assert(get_num_bits(bb_squares[A8])==1);
    assert(get_num_bits(bb_files[FILE_G])==8);
}

void test_get_lsb() {
    assert(get_lsb(bb_squares[E4])==E4);
    assert(get_lsb(bb_ranks[RANK_6])==A6);
}

void test_get_msb() {
    assert(get_msb(bb_squares[E4])==E4);
    assert(get_msb(bb_ranks[RANK_6])==H6);
}

void test_isolate_lsb_bit() {
    uint64 mask = bb_squares[A8] | bb_squares[E4] | bb_squares[F4] | bb_squares[D1];

    assert(isolate_lsb_bit(mask,0)==bb_squares[A8]);
    assert(isolate_lsb_bit(mask,1)==bb_squares[E4]);
    assert(isolate_lsb_bit(mask,2)==bb_squares[F4]);
    assert(isolate_lsb_bit(mask,3)==bb_squares[D1]);
}


void test_knight_moves() {
    assert(get_num_bits(bb_knight_moves[E4])==8);

    uint64 mv_map = bb_knight_moves[E4];
    assert(get_lsb(mv_map)==D6);
    mv_map ^= bb_squares[D6];

    assert(get_lsb(mv_map)==F6);
    mv_map ^= bb_squares[F6];

    assert(get_lsb(mv_map)==C5);
    mv_map ^= bb_squares[C5];

    assert(get_lsb(mv_map)==G5);
    mv_map ^= bb_squares[G5];

    assert(get_lsb(mv_map)==C3);
    mv_map ^= bb_squares[C3];

    assert(get_lsb(mv_map)==G3);
    mv_map ^= bb_squares[G3];

    assert(get_lsb(mv_map)==D2);
    mv_map ^= bb_squares[D2];

    assert(get_lsb(mv_map)==F2);
    mv_map ^= bb_squares[F2];

    assert(mv_map==0);

    assert(get_num_bits(bb_knight_moves[A8])==2);
    mv_map = bb_knight_moves[A8];
    assert(get_lsb(mv_map)==C7);
    mv_map ^= bb_squares[C7];

    assert(get_lsb(mv_map)==B6);
    mv_map ^= bb_squares[B6];

    assert(mv_map==0);
}

void test_king_moves() {
    assert(get_num_bits(bb_king_moves[E4])==8);

    uint64 mv_map = bb_king_moves[E4];
    assert(get_lsb(mv_map)==D5);
    mv_map ^= bb_squares[D5];

    assert(get_lsb(mv_map)==E5);
    mv_map ^= bb_squares[E5];

    assert(get_lsb(mv_map)==F5);
    mv_map ^= bb_squares[F5];

    assert(get_lsb(mv_map)==D4);
    mv_map ^= bb_squares[D4];

    assert(get_lsb(mv_map)==F4);
    mv_map ^= bb_squares[F4];

    assert(get_lsb(mv_map)==D3);
    mv_map ^= bb_squares[D3];

    assert(get_lsb(mv_map)==E3);
    mv_map ^= bb_squares[E3];

    assert(get_lsb(mv_map)==F3);
    mv_map ^= bb_squares[F3];

    assert(mv_map==0);

    assert(get_num_bits(bb_king_moves[A8])==3);
    mv_map = bb_king_moves[A8];
    assert(get_lsb(mv_map)==B8);
    mv_map ^= bb_squares[B8];

    assert(get_lsb(mv_map)==A7);
    mv_map ^= bb_squares[A7];

    assert(get_lsb(mv_map)==B7);
    mv_map ^= bb_squares[B7];

    assert(mv_map==0);
}

void test_pawn_attacks() {
    assert(bb_pawn_attacks[E4][WHITE]==(bb_squares[D5] | bb_squares[F5]));
    assert(bb_pawn_attacks[E4][BLACK]==(bb_squares[D3] | bb_squares[F3]));

    assert(bb_pawn_attacks[A2][WHITE]==bb_squares[B3]);
    assert(bb_pawn_attacks[A2][BLACK]==bb_squares[B1]);

    assert(bb_pawn_attacks[H7][WHITE]==bb_squares[G8]);
    assert(bb_pawn_attacks[H7][BLACK]==bb_squares[G6]);
}

void test_rays() {
    assert(bb_rays[D5][NORTH] == (bb_squares[D6] | bb_squares[D7] | bb_squares[D8]));
    assert(bb_rays[D5][SOUTH] == (bb_squares[D4] | bb_squares[D3] | bb_squares[D2] | bb_squares[D1]));
    assert(bb_rays[D5][WEST] == (bb_squares[C5] | bb_squares[B5] | bb_squares[A5]));
    assert(bb_rays[D5][EAST] == (bb_squares[E5] | bb_squares[F5] | bb_squares[G5] | bb_squares[H5]));
    assert(bb_rays[D5][NORTHEAST] == (bb_squares[E6] | bb_squares[F7] | bb_squares[G8]));
    assert(bb_rays[D5][SOUTHEAST] == (bb_squares[E4] | bb_squares[F3] | bb_squares[G2] | bb_squares[H1]));
    assert(bb_rays[D5][SOUTHWEST] == (bb_squares[C4] | bb_squares[B3] | bb_squares[A2]));
    assert(bb_rays[D5][NORTHWEST] == (bb_squares[C6] | bb_squares[B7] | bb_squares[A8]));
}

void test_bitboards() {
    test_get_num_bits();
    test_get_lsb();
    test_get_msb();
    test_isolate_lsb_bit();
    test_knight_moves();
    test_king_moves();
    test_pawn_attacks();
    test_rays();
}
