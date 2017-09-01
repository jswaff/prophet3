/*
 * out.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: james
 */

#include <cassert>
#include <cstring>
#include "defs.h"
#include "globals.h"

void draw_board(position *p) {
	char text[100];

	for (int r=0;r<8;r++) {  // every rank
		strcpy(text,"");
		for (int f=0;f<8;f++) { // every file
			switch(p->piece[get_square((rank_t)r,(file_t)f)]) {
			case NO_PIECE: strcat(text,"- "); break;
			case PAWN: strcat(text,"P "); break;
			case -PAWN: strcat(text,"p "); break;
			case ROOK: strcat(text,"R "); break;
			case -ROOK: strcat(text,"r "); break;
			case BISHOP: strcat(text,"B "); break;
			case -BISHOP: strcat(text,"b "); break;
			case KNIGHT: strcat(text,"N "); break;
			case -KNIGHT: strcat(text,"n "); break;
			case QUEEN: strcat(text,"Q "); break;
			case -QUEEN: strcat(text,"q "); break;
			case KING: strcat(text,"K "); break;
			case -KING: strcat(text,"k "); break;
			}
		}

		// print the player to move
		if (r==RANK_6) {
			if (p->player==WHITE) {
				strcat(text,"   white to move");
			} else {
				strcat(text,"   black to move");
			}
		}

		// print the en passant square
		if (r==RANK_5) {
			if (p->ep_sq==NO_SQUARE) {
				strcat(text,"   no ep");
			} else {
				char buffer[3];
				sq_to_str(p->ep_sq,buffer);
				strcat(text,"   ep=");
				strcat(text,buffer);
			}
		}

		// print the castling rights on the fourth row
		if (r==RANK_4) {
			if (p->castling_rights) {
				strcat(text,"   castling=");
				if (can_castle_wk(p))
					strcat(text,"K");
				if (can_castle_wq(p))
					strcat(text,"Q");
				if (can_castle_bk(p))
					strcat(text,"k");
				if (can_castle_bq(p))
					strcat(text,"q");
			} else {
				strcat(text,"   no castling rights");
			}
		}

		strcat(text,"\n");
		print(text);
	}
}

void draw_bitboard(uint64 bb) {
	char text[100];
	strcpy(text,"");

	for (int i=0;i<64;i++) {
		if (bb & bb_squares[i]) {
			strcat(text,"1");
		} else {
			strcat(text,"0");
		}
		if (get_file((square_t)i)==FILE_H) {
			strcat(text,"\n");
		}
	}

	strcat(text,"\n");
	print(text);
}
