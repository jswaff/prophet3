/*
 * hash.cpp
 *
 *  Created on: Feb 2, 2016
 *      Author: James
 */

#include <cassert>
#include <cstdlib>
#include "defs.h"
#include "globals.h"

uint64 build_hash_key(position *pos) {
    uint64 hkey = 0;

    // fold in pieces
    for (int i=0;i<64;i++) {
        int piece = pos->piece[i];
        if (piece > NO_PIECE) {
            hkey ^= zkeys.pieces[piece][WHITE][i];
        } else if (piece < NO_PIECE) {
            hkey ^= zkeys.pieces[-piece][BLACK][i];
        }
    }

    // fold in player to move
    hkey ^= zkeys.ptm[pos->player];

    // castling rights
    assert(pos->castling_rights >= 0 && pos->castling_rights < 16);
    hkey ^= zkeys.casting_rights[pos->castling_rights];

    // ep square (including NO_SQUARE)
    assert(pos->ep_sq >= 0 && pos->ep_sq < 65);
    hkey ^= zkeys.ep[pos->ep_sq];

    return hkey;
}

uint64 build_pawn_key(position *pos) {
    uint64 pkey = 0;

    uint64 pmap = pos->white_pawns;
    while (pmap) {
        square_t sq = (square_t)get_msb(pmap);
        pkey ^= zkeys.pieces[PAWN][WHITE][sq];
        pmap ^= bb_squares[sq];
    }

    pmap = pos->black_pawns;
    while (pmap) {
        square_t sq = (square_t)get_lsb(pmap);
        pkey ^= zkeys.pieces[PAWN][BLACK][sq];
        pmap ^= bb_squares[sq];
    }

    return pkey;
}

uint64 build_hash_val(hash_entry_t entry_type,int32 depth,int32 score,move mv) {
    assert(entry_type >= LOWER_BOUND && entry_type <= EXACT_SCORE);
    assert(depth >= 0);
    assert(depth <= 0xFFFF);
    assert(abs(score) <= CHECKMATE);
    assert(abs(score) <= 0xFFFF);
    set_move_score(&mv,0);

    // convert mate scores
    if (score > CHECKMATE-500) {
        if (entry_type==UPPER_BOUND) {
            // failing low on mate.  don't allow a cutoff, just store any associated move
            entry_type = MOVE_ONLY;
        } else {
            // convert to fail high
            entry_type = LOWER_BOUND;
            score = CHECKMATE-500;
        }
    } else if (score < -(CHECKMATE-500)) {
        if (entry_type==LOWER_BOUND) {
            // failing high on -mate.
            entry_type = MOVE_ONLY;
        } else {
            // convert to fail low
            entry_type = UPPER_BOUND;
            score = -(CHECKMATE-500);
        }
    }

    uint64 val = entry_type;
    val |= ((uint64)depth) << 2;
    if (score >= 0) {
        val |= ((uint64)score) << 18;
    } else {
        val |= ((uint64)-score) << 18;
        val |= ((uint64)1) << 34;
    }
    val |= ((uint64)mv) << 35;

    return val;
}

uint64 build_pawn_hash_val(int32 score) {
    assert(abs(score) <= CHECKMATE);
    assert(abs(score) <= 0xFFFF);

    uint64 val = 1; // just so it's non-zero. this way we know there IS an entry in case
                    // the score is 0

    if (score >= 0) {
        val |= ((uint64)score) << 1;
    } else {
        val |= ((uint64)-score) << 1;
        val |= ((uint64)1) << 17;
    }

    return val;
}

hash_entry_t get_hash_entry_type(uint64 val) {

    return (hash_entry_t)(val & 3);
}

int32 get_hash_entry_depth(uint64 val) {

    return (int32)((val >> 2) & 0xFFFF); // 16 bit mask
}

int32 get_hash_entry_score(uint64 val) {
    int32 score = ((val >> 18) & 0xFFFF);
    if ((val >> 34) & 1) {
        return -score;
    } else {
        return score;
    }
}

int32 get_pawn_hash_entry_score(uint64 val) {
    int32 score = ((val >> 1) & 0xFFFF);
    if ((val >> 17) & 1) {
        return -score;
    } else {
        return score;
    }
}

move get_hash_entry_move(uint64 val) {

    return val >> 35;
}

void init_hash_table(hash_table *tbl,uint32 tblsize) {
    assert(tblsize > 0);

    int max_entries = tblsize / sizeof(hash_entry);
    printd("# max hash entries: %d\n",max_entries);

    // this loop will set # entries to a power of 2 > max_entries
    int actual_entries = 2;
    while (actual_entries <= max_entries) {
        actual_entries *= 2;
    }
    actual_entries /= 2;

    // set the mask and allocate memory
    tbl->tblmask = actual_entries - 1;
    printd("# effective hash entries: %d\n",actual_entries);

    tbl->tblptr = (hash_entry*)malloc(actual_entries * sizeof(hash_entry));
    printd("# effective table size: %d bytes\n\n",actual_entries * sizeof(hash_entry));
}

void clear_hash_table(hash_table *tbl) {
    assert(tbl->tblptr);
    hash_entry *he = tbl->tblptr;
    for (uint32 i=0;i<=tbl->tblmask;i++) {
        (he+i)->key = 0;
        (he+i)->val = 0;
    }
    tbl->probes = 0;
    tbl->hits = 0;
    tbl->collisions = 0;
}

uint64 get_hash_entry(hash_table *tbl,uint64 key) {
    assert(tbl->tblptr);
    tbl->probes++;
    hash_entry *he = tbl->tblptr + (key & tbl->tblmask);
    if (he->val != 0) {
        if (he->key == key) { // do full signature match
            tbl->hits++;
            return he->val;
        } else {
            tbl->collisions++;
        }
    }

    return 0;
}

void store_hash_entry(hash_table *tbl,uint64 key,uint64 val) {
    assert(tbl->tblptr);
    hash_entry *he = tbl->tblptr + (key & tbl->tblmask);
    he->key = key;
    he->val = val;
}
