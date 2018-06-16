/*
 * utils.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: james
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "defs.h"
#include "globals.h"

color_t opposite_player(color_t player) {
	return player == WHITE ? BLACK : WHITE;
}

square_t str_to_sq(const char *str_sq) {
	if (strlen(str_sq) != 2) {
		return NO_SQUARE;
	}
	int32 sq;

	// the first character should be a-h or A-H
	if (str_sq[0] >= 'a' && str_sq[0] <= 'h') {
		sq = str_sq[0] - 'a';
	} else if (str_sq[0] >= 'A' && str_sq[0] <= 'H') {
		sq = str_sq[0] - 'A';
	} else {
		return NO_SQUARE;
	}

	// the second character should be a digit 1-8
	if (str_sq[1] >= '1' && str_sq[1] <= '8') {
		sq += 8*(8-(str_sq[1]-'0'));
	} else {
		return NO_SQUARE;
	}

	return (square_t)sq;
}

void sq_to_str(square_t sq,char *buffer) {
	buffer[0]='a' + get_file(sq);
	buffer[1]='0' + (8-get_rank(sq));
	buffer[2]=0;
}

void line_to_str(move_line *line,char *line_buffer) {
	assert(line);
	assert(line_buffer);
	line_buffer[0]='\0';
	char move_buffer[5];
	for (int i=0;i<line->n;i++) {
		move_to_str(line->mv[i],move_buffer);
		if (i>0) {
			int len=strlen(line_buffer);
			line_buffer[len] = ' ';
			line_buffer[len+1] = '\0';
		}
		strcat(line_buffer,move_buffer);
	}
}

void move_to_str(move mv,char *buffer) {
	square_t from_sq = get_from_sq(mv);
	square_t to_sq = get_to_sq(mv);
	sq_to_str(from_sq,buffer);
	sq_to_str(to_sq,buffer+2);
	piece_t promo = get_promopiece(mv);
	if (promo != NO_PIECE) {
		if (promo==QUEEN) { buffer[4]='q'; }
		else if (promo==ROOK) { buffer[4]='r'; }
		else if (promo==BISHOP) { buffer[4]='b'; }
		else if (promo==KNIGHT) { buffer[4]='n'; }
		buffer[5]='\0';
	}
}

/**
 * Convert a string <str_mv> into a move.
 * If the move is not legal then BADMOVE is returned.
 *
 * Normal moves: e2e4
 * Pawn promotion: e7e8q
 * Castling: e1g1, e1c1, e8g8, e8c8
 */
move str_to_move(const char* str_mv,position *pos) {
	char *my_mv = strdup(str_mv);

	// get rid of the mate and check designators if present
	char *p = strchr(my_mv,'#');
	if (p) *p = 0;
	p = strchr(my_mv,'+');
	if (p) *p = 0;

	// try short castle
	if (!strcmp(my_mv,"o-o") || !strcmp(my_mv,"O-O") || !strcmp(my_mv,"0-0")) {
		move mv;
		if (pos->player==WHITE) {
			mv = to_move(KING,E1,G1);
		} else {
			mv = to_move(KING,E8,G8);
		}
		set_castle(&mv);
		if (is_legal_move(mv,pos)) {
			return mv;
		} else {
			return BADMOVE;
		}
	}

	// try long castle
	if (!strcmp(my_mv,"o-o-o") || !strcmp(my_mv,"O-O-O") || !strcmp(my_mv,"0-0-0")) {
		move mv;
		if (pos->player==WHITE) {
			mv = to_move(KING,E1,C1);
		} else {
			mv = to_move(KING,E8,C8);
		}
		set_castle(&mv);
		if (is_legal_move(mv,pos)) {
			return mv;
		} else {
			return BADMOVE;
		}
	}

	// remove the promotion indicator if present and set the promotion piece.
	size_t len = strlen(my_mv);
	piece_t promo = NO_PIECE;

	switch(my_mv[len-1]) {
	case 'R':
	case 'r':
		promo = ROOK;
		break;
	case 'B':
	case 'b':
		promo = BISHOP;
		break;
	case 'N':
	case 'n':
		promo = KNIGHT;
		break;
	case 'Q':
	case 'q':
		promo = QUEEN;
		break;
	}

	if (promo != NO_PIECE) {
		my_mv[len-1]=0;
	}
	p = strchr(my_mv,'=');
	if (p) *p = 0;

	// translate the destination
	len = strlen(my_mv);
	if (len < 2) { return BADMOVE; }
	if (my_mv[len-1] < '1' || my_mv[len-1] > '8') {
		return BADMOVE;
	}
	int32 to_sq = 8*(8-(my_mv[len-1]-'0'));
	my_mv[len-1]=0;
	len--;

	if (my_mv[len-1] < 'a' || my_mv[len-1] > 'h') {
		return BADMOVE;
	}
	to_sq += my_mv[len-1]-'a';
	my_mv[len-1]=0;
	len--;

	// set the piece
	piece_t piece = NO_PIECE;
	if (my_mv[0]=='R') {
		piece = ROOK;
	} else if (my_mv[0]=='N') {
		piece = KNIGHT;
	} else if (my_mv[0]=='B') {
		piece = BISHOP;
	} else if (my_mv[0]=='Q') {
		piece = QUEEN;
	} else if (my_mv[0]=='K') {
		piece = KING;
	} else if (my_mv[0]=='P') {
		piece = PAWN;
	}

	if (piece != NO_PIECE) {
		my_mv++;
	}

	// is this a capture?
	if (my_mv[strlen(my_mv)-1]=='x') {
		my_mv[strlen(my_mv)-1] = 0;
	}

	// if anything is left, it must be the source/rank designator(s)
	int32 sfile = -1;
	if (strlen(my_mv) && my_mv[0] >= 'a' && my_mv[0] <= 'h') {
		sfile = my_mv[0] - 'a';
		my_mv++;
	}

	int32 srank = -1;
	if (strlen(my_mv) && my_mv[0] >= '1' && my_mv[0] <= '8') {
		srank = '8' - my_mv[0];
		my_mv++;
	}

	// if the piece didn't get set earlier, do it now
	if (piece==NO_PIECE) {
		if (srank > -1 && sfile > -1) {
			piece = (piece_t)pos->piece[(srank*8)+sfile];
			if (piece < 0) {
				piece = (piece_t)(0-piece);
			}
		} else {
			piece = PAWN;
		}
	}

	// check against the list of legal moves
	move moves[MAX_MOVES_PER_PLY];
	move *endp = gen_legal_moves(moves,pos,true,true);

	int32 matches = 0;
	move mv = BADMOVE;

	for (move *mp=moves;mp<endp;mp++) {
		// is this move a match?
		if (srank > -1 && srank != get_rank(get_from_sq(*mp))) {
			continue;
		}
		if (sfile > -1 && sfile != get_file(get_from_sq(*mp))) {
			continue;
		}
		if (get_to_sq(*mp)==to_sq && get_piece(*mp)==piece
				&& get_promopiece(*mp)==promo) {
			matches++;
			mv = *mp;
		}
	}

	if (matches != 1) {
		mv = BADMOVE;
	}

	return mv;
}

bool is_line_valid(move_line *line,position *pos) {

	// copy the position
	position my_pos;
	memcpy(&my_pos,pos,sizeof(position));

	undo undos[pos->move_counter + line->n];

	for (int i=0;i<line->n;i++) {
		if (!is_legal_move(line->mv[i],&my_pos)) {
			print("invalid line!\n");
			draw_board(pos);
			return false;
		}
		apply_move(&my_pos,line->mv[i],undos);
	}

	return true;
}

bool is_legal_move(move mv,position *pos) {
	move moves[MAX_MOVES_PER_PLY];
	move *endp = gen_legal_moves(moves,pos,true,true);

	return is_in_move_list(mv,moves,endp);
}

/**
 * Is <mv> in the list [start,end) ?
 * The score portion of the move is ignored.
 */
bool is_in_move_list(move mv,move *start,move *end) {
	for (move *mp=start;mp<end;mp++) {
		if (clear_score(*mp) == clear_score(mv)) {
			return true;
		}
	}
	return false;
}

/**
 * Get the number of legal moves in this position.
 */
uint32 num_moves(position *pos,bool caps,bool noncaps) {
	move moves[MAX_MOVES_PER_PLY],*mp;
	mp = gen_legal_moves(moves,pos,caps,noncaps);
	return num_moves(moves,mp,caps,noncaps);
}

/**
 * Get the number of moves in the range [startp,endp).  Purged moves (those == 0) are not counted.
 */
uint32 num_moves(move *startp,move *endp,bool caps,bool noncaps) {
	uint32 n=0;
	for (move* mp=startp;mp<endp;mp++) {
		if (*mp != 0) {
			if (is_capture(*mp) || get_promopiece(*mp)) {
				if (caps) n++;
			} else {
				if (noncaps) n++;
			}
		}
	}
	return n;
}

bool is_white_piece(int32 piece) {
	return piece > NO_PIECE;
}

bool is_black_piece(int32 piece) {
	return piece < NO_PIECE;
}

bool is_not_white_piece(int32 piece) {
	return piece <= NO_PIECE;
}

bool is_not_black_piece(int32 piece) {
	return piece >= NO_PIECE;
}

game_status get_game_status() {
	if (is_checkmate(&gpos)) return CHECKMATED;
	if (is_stalemate(&gpos)) return STALEMATED;
	if (is_lack_of_mating_material(&gpos)) return DRAW_MATERIAL;
	if (is_draw_by_rep(&gpos,gundos)) return DRAW_REP;
	if (is_draw_by_50(&gpos)) return DRAW_BY_50;

	return INPROGRESS;
}

void print_move(move mv) {
	char mv_buffer[MAX_CHARS_PER_MOVE];
	move_to_str(mv,mv_buffer);
	print("move %s\n",mv_buffer);
}

/*
 * unsigned int Random32(void)
 *
 * The 32 bit random number generator below was taken from Crafty by Bob
 * Hyatt.  This same algorithm is in GNU Chess, so I'm not sure where it
 * originated from.
 *
 * The following comments are from Crafty:
 * ---------------------
 * A 32 bit random number generator. An implementation in C of the algorithm
 * given by Knuth, the art of computer programming, vol. 2, pp. 26-27. We use
 * e=32, so we have to evaluate y(n) = y(n - 24) + y(n - 55) mod 2^32, which
 * is implicitly done by unsigned arithmetic.
 *
 */
uint32 random32() {
	/*
	  random numbers from Mathematica 2.0.
	  SeedRandom = 1;
	  Table[Random[Integer, {0, 2^32 - 1}]
	 */
	static unsigned long x[55] = {
			1410651636UL, 3012776752UL, 3497475623UL, 2892145026UL, 1571949714UL,
			3253082284UL, 3489895018UL, 387949491UL, 2597396737UL, 1981903553UL,
			3160251843UL, 129444464UL, 1851443344UL, 4156445905UL, 224604922UL,
			1455067070UL, 3953493484UL, 1460937157UL, 2528362617UL, 317430674UL,
			3229354360UL, 117491133UL, 832845075UL, 1961600170UL, 1321557429UL,
			747750121UL, 545747446UL, 810476036UL, 503334515UL, 4088144633UL,
			2824216555UL, 3738252341UL, 3493754131UL, 3672533954UL, 29494241UL,
			1180928407UL, 4213624418UL, 33062851UL, 3221315737UL, 1145213552UL,
			2957984897UL, 4078668503UL, 2262661702UL, 65478801UL, 2527208841UL,
			1960622036UL, 315685891UL, 1196037864UL, 804614524UL, 1421733266UL,
			2017105031UL, 3882325900UL, 810735053UL, 384606609UL, 2393861397UL };

	static int init = 1;
	static unsigned long y[55];
	static int j, k;
	unsigned long ul;

	if (init) {
		int i;

		init = 0;
		for (i = 0; i < 55; i++) {
			y[i] = x[i];
		}
		j = 24 - 1;
		k = 55 - 1;
	}

	ul = (y[k] += y[j]);
	if (--j < 0) {
		j = 55 - 1;
	}
	if (--k < 0) {
		k = 55 - 1;
	}

	return((uint32)ul);
}

uint64 random64() {
	uint64 r1 = random32();
	uint64 r2 = random32();
	return ((r1<<32)|r2);
}

