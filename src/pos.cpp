/*
 * pos.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: james
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "defs.h"
#include "globals.h"

const unsigned int CASTLE_WK     = 1;
const unsigned int CASTLE_WQ     = 2;
const unsigned int CASTLE_BK     = 4;
const unsigned int CASTLE_BQ     = 8;
const unsigned int CASTLE_WHITE  = 3;
const unsigned int CASTLE_NOT_WK =14;
const unsigned int CASTLE_NOT_WQ =13;
const unsigned int CASTLE_NOT_BK =11;
const unsigned int CASTLE_NOT_BQ = 7;
const unsigned int CASTLE_BLACK  =12;
const unsigned int CASTLE_ALL    =15;

bool equal_pos(position* p1,position* p2,bool strict) {
	for (int32 sq=0;sq<64;sq++) {
		if (p1->piece[sq] != p2->piece[sq]) {
			return false;
		}
	}
	assert(p1->white_pawns==p2->white_pawns);
	assert(p1->black_pawns==p2->black_pawns);

	assert(p1->white_knights==p2->white_knights);
	assert(p1->black_knights==p2->black_knights);

	assert(p1->white_bishops==p2->white_bishops);
	assert(p1->black_bishops==p2->black_bishops);

	assert(p1->white_rooks==p2->white_rooks);
	assert(p1->black_rooks==p2->black_rooks);

	assert(p1->white_queens==p2->white_queens);
	assert(p1->black_queens==p2->black_queens);

	assert(p1->white_pieces==p2->white_pieces);
	assert(p1->black_pieces==p2->black_pieces);

	// if that's true then piece counts should be equal
	assert(p1->piece_counts[WHITE][PAWN]==p2->piece_counts[WHITE][PAWN]);
	assert(p1->piece_counts[WHITE][ROOK]==p2->piece_counts[WHITE][ROOK]);
	assert(p1->piece_counts[WHITE][KNIGHT]==p2->piece_counts[WHITE][KNIGHT]);
	assert(p1->piece_counts[WHITE][BISHOP]==p2->piece_counts[WHITE][BISHOP]);
	assert(p1->piece_counts[WHITE][QUEEN]==p2->piece_counts[WHITE][QUEEN]);

	assert(p1->piece_counts[BLACK][PAWN]==p2->piece_counts[BLACK][PAWN]);
	assert(p1->piece_counts[BLACK][ROOK]==p2->piece_counts[BLACK][ROOK]);
	assert(p1->piece_counts[BLACK][KNIGHT]==p2->piece_counts[BLACK][KNIGHT]);
	assert(p1->piece_counts[BLACK][BISHOP]==p2->piece_counts[BLACK][BISHOP]);
	assert(p1->piece_counts[BLACK][QUEEN]==p2->piece_counts[BLACK][QUEEN]);

	if (p1->player != p2->player) return false;
	if (p1->ep_sq != p2->ep_sq) return false;
	if (p1->castling_rights != p2->castling_rights) return false;
	if (p1->white_king != p2->white_king) return false;
	if (p1->black_king != p2->black_king) return false;
	if (strict) {
		if (p1->move_counter != p2->move_counter) return false;
		if (p1->fifty_counter != p2->fifty_counter) return false;
	}

	// if all that is equal the hash keys should be too
	assert(p1->hash_key==p2->hash_key);
	assert(p1->pawn_key==p2->pawn_key);

	return true;
}

void add_piece(position* p,int32 piece,square_t sq) {
	assert(piece != NO_PIECE);
	assert(sq >= 0 && sq < 64);
	assert(p->piece[sq]==NO_PIECE);

	p->piece[sq] = piece;
	uint64 bb_sq = bb_squares[sq];
	if (piece > NO_PIECE) {
		p->white_pieces |= bb_sq;
		if (piece==PAWN) {
			p->white_pawns |= bb_sq;
			p->pawn_key ^= zkeys.pieces[PAWN][WHITE][sq];
		} else if (piece==KNIGHT) {
			p->white_knights |= bb_sq;
		} else if (piece==BISHOP) {
			p->white_bishops |= bb_sq;
		} else if (piece==ROOK) {
			p->white_rooks |= bb_sq;
		} else if (piece==QUEEN) {
			p->white_queens |= bb_sq;
		}
		p->piece_counts[WHITE][piece]++;
		p->hash_key ^= zkeys.pieces[piece][WHITE][sq];
	} else {
		p->black_pieces |= bb_sq;
		if (piece==-PAWN) {
			p->black_pawns |= bb_sq;
			p->pawn_key ^= zkeys.pieces[PAWN][BLACK][sq];
		} else if (piece==-KNIGHT) {
			p->black_knights |= bb_sq;
		} else if (piece==-BISHOP) {
			p->black_bishops |= bb_sq;
		} else if (piece==-ROOK) {
			p->black_rooks |= bb_sq;
		} else if (piece==-QUEEN) {
			p->black_queens |= bb_sq;
		}
		p->piece_counts[BLACK][-piece]++;
		p->hash_key ^= zkeys.pieces[-piece][BLACK][sq];
	}
}

piece_t remove_piece(position* p,square_t sq) {
	assert(p != 0);
	assert(sq >= A8 && sq <= H1);
	assert(p->piece[sq] != NO_PIECE);

	int piece = p->piece[sq];
	uint64 bb_sq = bb_squares[sq];
	if (piece > NO_PIECE) {
		p->white_pieces ^= bb_sq;
		if (piece==PAWN) {
			p->white_pawns ^= bb_sq;
			p->pawn_key ^= zkeys.pieces[PAWN][WHITE][sq];
		} else if (piece==KNIGHT) {
			p->white_knights ^= bb_sq;
		} else if (piece==BISHOP) {
			p->white_bishops ^= bb_sq;
		} else if (piece==ROOK) {
			p->white_rooks ^= bb_sq;
		} else if (piece==QUEEN) {
			p->white_queens ^= bb_sq;
		}
		p->piece_counts[WHITE][piece]--;
		assert(p->piece_counts[WHITE][piece] >= 0);
		p->hash_key ^= zkeys.pieces[piece][WHITE][sq];
	} else {
		p->black_pieces ^= bb_sq;
		if (piece==-PAWN) {
			p->black_pawns ^= bb_sq;
			p->pawn_key ^= zkeys.pieces[PAWN][BLACK][sq];
		} else if (piece==-KNIGHT) {
			p->black_knights ^= bb_sq;
		} else if (piece==-BISHOP) {
			p->black_bishops ^= bb_sq;
		} else if (piece==-ROOK) {
			p->black_rooks ^= bb_sq;
		} else if (piece==-QUEEN) {
			p->black_queens ^= bb_sq;
		}
		p->piece_counts[BLACK][-piece]--;
		assert(p->piece_counts[BLACK][-piece] >= 0);
		p->hash_key ^= zkeys.pieces[-piece][BLACK][sq];
	}

	p->piece[sq] = NO_PIECE;
	return (piece_t)piece;
}

bool can_castle_wk(position *pos) {
	return pos->castling_rights & CASTLE_WK;
}

bool can_castle_wq(position *pos) {
	return pos->castling_rights & CASTLE_WQ;
}

bool can_castle_bk(position *pos) {
	return pos->castling_rights & CASTLE_BK;
}

bool can_castle_bq(position *pos) {
	return pos->castling_rights & CASTLE_BQ;
}

void remove_rook_castling_availability(position *p,square_t sq) {
	if (sq==A1) {
		p->castling_rights &= CASTLE_NOT_WQ;
	} else if (sq==H1) {
		p->castling_rights &= CASTLE_NOT_WK;
	} else if (sq==A8) {
		p->castling_rights &= CASTLE_NOT_BQ;
	} else if (sq==H8) {
		p->castling_rights &= CASTLE_NOT_BK;
	}
}

void remove_castling_availability(position *p,move mv) {
	// if capturing a rook remove its castling availability
	if (is_capture(mv)) {
		square_t to_sq = get_to_sq(mv);
		remove_rook_castling_availability(p,to_sq);
	}

	// if a rook or king is moving, remove their castling availability.
	square_t from_sq = get_from_sq(mv);
	int32 piece = p->piece[from_sq];
	if (piece==KING) {
		p->castling_rights &= CASTLE_BLACK;
	} else if (piece==-KING) {
		p->castling_rights &= CASTLE_WHITE;
	} else if (piece==ROOK || piece==-ROOK) {
		remove_rook_castling_availability(p,from_sq);
	}
}


/**
 * Set the board to the initial position.
 */
void reset_pos(position *p) {
	assert(p!=0);
	set_pos(p,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

/**
 *
 * From : http://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
 * A FEN record contains six fields. The separator between fields is a space. The fields are:

    1. Piece placement (from white's perspective). Each rank is described, starting with rank 8 and
       ending with rank 1; within each rank, the contents of each square are described from file a
       through file h. Following the Standard Algebraic Notation (SAN), each piece is identified by a
       single letter taken from the standard English names (pawn = "P", knight = "N", bishop = "B",
       rook = "R", queen = "Q" and king = "K").[1] White pieces are designated using upper-case
       letters ("PNBRQK") while black pieces use lowercase ("pnbrqk"). Blank squares are noted using
       digits 1 through 8 (the number of blank squares), and "/" separate ranks.

    2. Active color. "w" means white moves next, "b" means black.

    3. Castling availability. If neither side can castle, this is "-". Otherwise, this has one or more
       letters: "K" (White can castle kingside), "Q" (White can castle queenside), "k" (Black can
       castle kingside), and/or "q" (Black can castle queenside).

    4. En passant target square in algebraic notation. If there's no en passant target square, this
       is "-". If a pawn has just made a 2-square move, this is the position "behind" the pawn. This
       is recorded regardless of whether there is a pawn in position to make an en passant capture.

    5. Halfmove clock: This is the number of halfmoves since the last pawn advance or capture. This is
       used to determine if a draw can be claimed under the fifty-move rule.

    6. Fullmove number: The number of the full move. It starts at 1, and is incremented after Black's
       move.
 *
 */
bool set_pos(position *pos,const char *fen) {
	char *my_fen = strdup(fen),*fen_part;
	int32 sq;

	for (sq=A8;sq<=H1;sq++) {
		pos->piece[sq] = NO_PIECE;
	}
	sq = A8;

	for (int32 i=NO_PIECE;i<=KING;i++) {
		pos->piece_counts[WHITE][i] = 0;
		pos->piece_counts[BLACK][i] = 0;
	}

	pos->white_pawns = pos->white_knights = pos->white_bishops = pos->white_rooks
			= pos->white_queens = pos->white_pieces = 0;
	pos->black_pawns = pos->black_knights = pos->black_bishops = pos->black_rooks
			= pos->black_queens = pos->black_pieces = 0;

	// Part 1 - Piece Placement
	fen_part = strtok(my_fen," ");
	for (uint32 c=0;c<strlen(fen_part);c++) {
		char ch = *(fen_part+c);
		if (ch == '/') continue;
		if (ch >= '1' && ch <= '8') {
			sq += ((int)ch)-48;  // '1' == ASC 48, '8' == ASC 56
		} else {
			int32 piece=NO_PIECE;
			if (ch=='K') {
				pos->white_king = (square_t)sq;
				piece = KING;
			} else if (ch=='k') {
				pos->black_king = (square_t)sq;
				piece = -KING;
			} else if (ch=='Q') {
				piece = QUEEN;
			} else if (ch=='q') {
				piece = -QUEEN;
			} else if (ch=='R') {
				piece = ROOK;
			} else if (ch=='r') {
				piece = -ROOK;
			} else if (ch=='B') {
				piece = BISHOP;
			} else if (ch=='b') {
				piece = -BISHOP;
			} else if (ch=='N') {
				piece = KNIGHT;
			} else if (ch=='n') {
				piece = -KNIGHT;
			} else if (ch=='P') {
				piece = PAWN;
			} else if (ch=='p') {
				piece = -PAWN;
			}
			assert(piece != NO_PIECE);
			add_piece(pos,piece,(square_t)sq);
			sq++;
		}
	}
	assert(sq==64);
	// Part 2 - Active Color
	fen_part = strtok(NULL," ");
	if (*fen_part=='w' || *fen_part=='W') {
		pos->player = WHITE;
	} else if (*fen_part=='b' || *fen_part=='B') {
		pos->player = BLACK;
	} else {
		error("active color - fen: %s\n",fen);
		return false;
	}

	// Part 3 - Castling Availability
	fen_part = strtok(NULL," ");
	if (!fen_part) {
		error("castling availability - fen: %s\n",fen);
		return false;
	}
	pos->castling_rights = 0;
	if (strchr(fen_part,'K'))
		pos->castling_rights |= CASTLE_WK;
	if (strchr(fen_part,'Q'))
		pos->castling_rights |= CASTLE_WQ;
	if (strchr(fen_part,'k'))
		pos->castling_rights |= CASTLE_BK;
	if (strchr(fen_part,'q'))
		pos->castling_rights |= CASTLE_BQ;

	// Part 4 - En Passant Target Square
	fen_part = strtok(NULL," ");
	if (!fen_part) {
		error("en passant target square - fen: %s\n",fen);
		return false;
	}
	pos->ep_sq = str_to_sq(fen_part);

	// Part 5 - Halfmove clock (Note: not part of the EPD spec)
	fen_part = strtok(NULL," ");
	if (fen_part) {
		pos->fifty_counter=atoi(fen_part);
	} else {
		pos->fifty_counter=0;
	}

	// Part 6 - Full Move Counter (Note: not part of the EPD spec)
	// It starts at 1, and is incremented after each Black's move.
	fen_part = strtok(NULL," ");
	if (fen_part) {
		pos->move_counter=(atoi(fen_part)-1)*2;
	} else {
		pos->move_counter=0;
	}
	if (pos->player==BLACK) pos->move_counter++;

	pos->hash_key = build_hash_key(pos);
	pos->pawn_key = build_pawn_key(pos);

	return true;
}

bool is_empty_sq(position *pos,square_t sq) {
	assert(sq >= A8 && sq <= H1);
	return pos->piece[sq] == NO_PIECE;
}

bool is_enemy_occupied(position *pos,square_t sq,color_t player) {
	assert(pos);
	assert(sq >= A8 && sq <= H1);
	assert(player==WHITE || player==BLACK);

	return player==WHITE ? is_black_piece(pos->piece[sq]) : is_white_piece(pos->piece[sq]);
}

bool is_friendly_occupied(position *pos,square_t sq,color_t player) {
	assert(pos);
	assert(sq >= A8 && sq <= H1);
	assert(player==WHITE || player==BLACK);

	return player==WHITE ? is_white_piece(pos->piece[sq]) : is_black_piece(pos->piece[sq]);
}

bool in_check(position *pos,color_t player) {
	square_t king_sq = player==WHITE ? pos->white_king : pos->black_king;
	return attacked(pos,king_sq,opposite_player(player));
}

/**
 * Flipping the board puts white in blacks position, but with the board flipped
 * vertically.
 */
void flip_board(position *pos) {

	// flip pieces around
	int32 orig_sq[64];
	for (int sq=0;sq<64;sq++) {
		orig_sq[sq] = pos->piece[sq];
		if (pos->piece[sq] != NO_PIECE) {
			remove_piece(pos,(square_t)sq);
		}
	}

	for (int sq=0;sq<64;sq++) {
		if (orig_sq[sq] != NO_PIECE) {
			add_piece(pos,-orig_sq[sq],(square_t)fliprank[sq]);
		}
	}

	square_t orig_white_king_sq = pos->white_king;
	pos->white_king = (square_t)fliprank[pos->black_king];
	pos->black_king = (square_t)fliprank[orig_white_king_sq];

	// swap players
	pos->player = opposite_player(pos->player);

	// flip ep square
	if (pos->ep_sq != NO_SQUARE) {
		pos->ep_sq = (square_t)fliprank[pos->ep_sq];
	}

	// castling rights
	int32 orig_castling_rights = pos->castling_rights;
	pos->castling_rights = 0;
	if (orig_castling_rights & CASTLE_WK) {
		pos->castling_rights |= CASTLE_BK;
	}
	if (orig_castling_rights & CASTLE_WQ) {
		pos->castling_rights |= CASTLE_BQ;
	}
	if (orig_castling_rights & CASTLE_BK) {
		pos->castling_rights |= CASTLE_WK;
	}
	if (orig_castling_rights & CASTLE_BQ) {
		pos->castling_rights |= CASTLE_WQ;
	}

	pos->hash_key = build_hash_key(pos);
	pos->pawn_key = build_pawn_key(pos);
	assert(test_pos_consistency(pos));
}

/*
 * Is the current player in checkmate?  Yes if two things are true:
 * 1. They are in check
 * 2. There are no legal moves
 */
bool is_checkmate(position *pos) {
	assert(pos);
	return in_check(pos,pos->player) && num_moves(pos,true,true)==0;
}

/**
 * Is the current player in stalemate?  Yes if two things are true:
 * 1. NOT in check
 * 2. No legal moves
 */
bool is_stalemate(position *pos) {
	assert(pos);
	return !in_check(pos,pos->player) && num_moves(pos,true,true)==0;
}

bool is_zugzwang(position *pos) {

	if (pos->piece_counts[WHITE][ROOK] + pos->piece_counts[WHITE][KNIGHT]
	       + pos->piece_counts[WHITE][BISHOP] + pos->piece_counts[WHITE][QUEEN] == 0)
	{
		return true;
	}

	if (pos->piece_counts[BLACK][ROOK] + pos->piece_counts[BLACK][KNIGHT]
	       + pos->piece_counts[BLACK][BISHOP] + pos->piece_counts[BLACK][QUEEN] == 0)
	{
		return true;
	}


	return false;
}

