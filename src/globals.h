/*
 * globals.h
 *
 *  Created on: Feb 1, 2011
 *      Author: james
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <iostream>
#include <sqlite3.h>
#include "defs.h"

extern sqlite3* book_db;
extern dir_t direction_tbl[64][64];
extern position gpos;
extern undo gundos[UNDO_STACK_SIZE];
extern hash_table htbl;
extern hash_table phtbl;
extern move killer1[MAX_PLY];
extern move killer2[MAX_PLY];
extern pthread_t think_thread;
extern zobrist_keys zkeys;

extern uint64 bb_files[8];
extern uint64 bb_ranks[8];
extern uint64 bb_squares[64];
extern uint64 bb_knight_moves[64];
extern uint64 bb_king_moves[64];
extern uint64 bb_pawn_attacks[64][2];
extern uint64 bb_rays[64][8];

//// constants

const uint32 default_hash_size = 134217728 * 2; // default 256 MB

const int fliprank[64] =
    { A1,B1,C1,D1,E1,F1,G1,H1,
      A2,B2,C2,D2,E2,F2,G2,H2,
      A3,B3,C3,D3,E3,F3,G3,H3,
      A4,B4,C4,D4,E4,F4,G4,H4,
      A5,B5,C5,D5,E5,F5,G5,H5,
      A6,B6,C6,D6,E6,F6,G6,H6,
      A7,B7,C7,D7,E7,F7,G7,H7,
      A8,B8,C8,D8,E8,F8,G8,H8  };



#endif /* GLOBALS_H_ */
