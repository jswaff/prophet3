/*
 * globals.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: james
 */

#include <iostream>
#include "defs.h"
#include "sqlite3.h"

sqlite3* book_db;
dir_t direction_tbl[64][64];
position gpos;
undo gundos[UNDO_STACK_SIZE];
hash_table htbl;
hash_table phtbl;
move killer1[MAX_PLY];
move killer2[MAX_PLY];
pthread_t think_thread;
zobrist_keys zkeys;

uint64 bb_files[8];
uint64 bb_ranks[8];
uint64 bb_squares[64];
uint64 bb_knight_moves[64];
uint64 bb_king_moves[64];
uint64 bb_pawn_attacks[64][2];
uint64 bb_rays[64][8];
