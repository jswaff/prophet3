/*
 * eval.h
 *
 *  Created on: Dec 26, 2011
 *      Author: james
 */

#ifndef EVAL_H_
#define EVAL_H_

const int32 pawn_val = 100;
const int32 knight_val = 300;
const int32 bishop_val = 320;
const int32 rook_val = 500;
const int32 queen_val = 900;
const int32 all_nonpawn_pieces_val = queen_val + rook_val*2 + bishop_val*2 + knight_val*2;

//enum piece_t { NO_PIECE,PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING };
const int32 pvals[13] = { INF,queen_val,rook_val,bishop_val,knight_val,pawn_val,0,
		pawn_val,knight_val,bishop_val,rook_val,queen_val,INF };


// having majors on the 7th is huge advantage.  This might actually be too small.
const int32 rook_on_7th = 50;
const int32 connected_majors_on_7th = 80;

// an open file is one with no pawns of either color on it
const int32 rook_open_file = 25;

// a half-open file is one with enemy pawns but not our own
const int32 rook_half_open_file = 15;

// a passed pawn is a pawn with no enemy pawn in front of it or on an adjacent file
const int32 passed_pawn = 20;

// an isolated pawn is one with no friendly pawn on an adjacent file
const int32 isolated_pawn = -20;

// a doubled pawn is a pawn that resides on the same file as a friendly pawn
// note this would get "awarded" to both pawns
const int32 doubled_pawn = -10;

const int32 king_safety_pawn_one_away = -10;
const int32 king_safety_pawn_two_away = -20;
const int32 king_safety_pawn_far_away = -30;
const int32 king_safety_middle_open_file = -50;

const int knight_tropism = -1;

const int bishop_pst[64] = {
	  0, 0,  0,  0,  0,  0, 0, 0,
	  0, 7,  7,  7,  7,  7, 7, 0,
	  0, 7, 15, 15, 15, 15, 7, 0,
	  0, 7, 15, 20, 20, 15, 7, 0,
	  0, 7, 15, 20, 20, 15, 7, 0,
	  0, 7, 15, 15, 15, 15, 7, 0,
	  0, 7,  7,  7,  7,  7, 7, 0,
	  0, 0,  0,  0,  0,  0, 0, 0 };

const int knight_pst[64] = {
	 -5, -5, -5, -5, -5, -5, -5, -5,
	 -5,  0, 10, 10, 10, 10,  0, -5,
	 -5,  0, 15, 20, 20, 15,  0, -5,
	 -5,  5, 10, 15, 15, 10,  5, -5,
	 -5,  0, 10, 15, 15, 10,  5, -5,
	 -5,  0,  8,  0,  0,  8,  0, -5,
	 -5,  0,  0,  5,  5,  0,  0, -5,
	-10,-10, -5, -5, -5, -5,-10,-10 };

const int pawn_pst[64] = {
	 0,  0,  0,  0,  0,  0,  0,  0,
	30, 30, 30, 30, 30, 30, 30, 30,
	14, 14, 14, 18, 18, 14, 14, 14,
	 7,  7,  7, 10, 10,  7,  7,  7,
	 5,  5,  5,  7,  7,  5,  5,  5,
	 3,  3,  3,  5,  5,  3,  3,  3,
	 0,  0,  0, -3, -3,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0 };

const int rook_pst[64] = {
	 0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	 0,  0,  0,  0,  0,  0,  0,  0 };

const int king_pst[64] = {
   -30,-30,-30,-30,-30,-30,-30,-30,
   -30,-30,-30,-30,-30,-30,-30,-30,
   -30,-30,-30,-30,-30,-30,-30,-30,
   -30,-30,-30,-30,-30,-30,-30,-30,
   -30,-30,-30,-30,-30,-30,-30,-30,
   -20,-20,-20,-20,-20,-20,-20,-20,
   -10,-10,-10,-10,-10,-10,-10,-10,
     0, 10, 20,-25,  0,-25, 20,  0 };

const int king_endgame_pst[64] = {
	 0,  0,  0,  0,  0,  0,  0,  0,
	 0, 10, 10, 10, 10, 10, 10,  0,
	 0, 10, 20, 20, 20, 20, 10,  0,
	 0, 10, 20, 25, 25, 20, 10,  0,
	 0, 10, 20, 25, 25, 20, 10,  0,
	 0, 10, 20, 20, 20, 20, 10,  0,
	 0, 10, 10, 10, 10, 10, 10,  0,
	 0,  0,  0,  0,  0,  0,  0,  0 };

const int queen_pst[64] = {
	-1, -1, -1, -1, -1, -1, -1, -1,
	-1,  0,  0,  0,  0,  0,  0, -1,
	-1,  0,  1,  1,  1,  1,  0, -1,
	-1,  0,  1,  2,  2,  1,  0, -1,
	-1,  0,  1,  2,  2,  1,  0, -1,
	-1,  0,  1,  1,  1,  1,  0, -1,
	-1,  0,  0,  0,  0,  0,  0, -1,
	-1, -1, -1, -1, -1, -1, -1, -1 };

#endif /* EVAL_H_ */
