/*
 * square__test.cpp
 *
 *  Created on: Dec 20, 2011
 *      Author: james
 */

#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"

void test_get_dir() {
	assert(direction_tbl[E4][E6]==NORTH);
	assert(direction_tbl[D6][D1]==SOUTH);
	assert(direction_tbl[H1][A1]==WEST);
	assert(direction_tbl[A7][G7]==EAST);

	assert(direction_tbl[B3][E6]==NORTHEAST);
	assert(direction_tbl[A5][E1]==SOUTHEAST);
	assert(direction_tbl[H3][G2]==SOUTHWEST);
	assert(direction_tbl[G5][E7]==NORTHWEST);

	assert(direction_tbl[C3][D5]==NODIR);
	assert(direction_tbl[E4][E4]==NODIR);
}

void test_rank() {
	assert(get_rank(A8)==RANK_8);
	assert(get_rank(B8)==RANK_8);
	assert(get_rank(H8)==RANK_8);
	assert(get_rank(C7)==RANK_7);
	assert(get_rank(E5)==RANK_5);
	assert(get_rank(A2)==RANK_2);
	assert(get_rank(H1)==RANK_1);
}

void test_file() {
	assert(get_file(A1)==FILE_A);
	assert(get_file(B3)==FILE_B);
	assert(get_file(C8)==FILE_C);
	assert(get_file(D4)==FILE_D);
	assert(get_file(E2)==FILE_E);
	assert(get_file(F7)==FILE_F);
	assert(get_file(G5)==FILE_G);
	assert(get_file(H6)==FILE_H);
}

void test_file_distance() {
	assert(file_distance(A1,B1)==1);
	assert(file_distance(A1,H8)==7);
	assert(file_distance(A1,H4)==7);
	assert(file_distance(B3,C7)==1);
	assert(file_distance(H2,A3)==7);
	assert(file_distance(F3,B8)==4);
	assert(file_distance(E1,E2)==0);
}

void test_rank_distance() {
	assert(rank_distance(A1,B1)==0);
	assert(rank_distance(A1,C4)==3);
	assert(rank_distance(H8,A7)==1);
	assert(rank_distance(G8,B1)==7);
}

void test_north() {
	assert(north(NO_SQUARE)==NO_SQUARE);
	assert(north(B4)==B5);
	assert(north(B8)==NO_SQUARE);
}

void test_east() {
	assert(east(NO_SQUARE)==NO_SQUARE);
	assert(east(B4)==C4);
	assert(east(H4)==NO_SQUARE);
}

void test_south() {
	assert(south(NO_SQUARE)==NO_SQUARE);
	assert(south(B4)==B3);
	assert(south(B1)==NO_SQUARE);
}

void test_west() {
	assert(west(NO_SQUARE)==NO_SQUARE);
	assert(west(B4)==A4);
	assert(west(A4)==NO_SQUARE);
}

void test_northeast() {
	assert(northeast(NO_SQUARE)==NO_SQUARE);
	assert(northeast(B4)==C5);
	assert(northeast(G8)==NO_SQUARE);
	assert(northeast(H7)==NO_SQUARE);
}

void test_southeast() {
	assert(southeast(NO_SQUARE)==NO_SQUARE);
	assert(southeast(B4)==C3);
	assert(southeast(G1)==NO_SQUARE);
	assert(southeast(H2)==NO_SQUARE);
}

void test_southwest() {
	assert(southwest(NO_SQUARE)==NO_SQUARE);
	assert(southwest(B4)==A3);
	assert(southwest(B1)==NO_SQUARE);
	assert(southwest(A2)==NO_SQUARE);
}

void test_northwest() {
	assert(northwest(NO_SQUARE)==NO_SQUARE);
	assert(northwest(B4)==A5);
	assert(northwest(A7)==NO_SQUARE);
	assert(northwest(B8)==NO_SQUARE);
}

void test_is_light_sq() {
	assert(is_light_sq(A8));
	assert(!is_light_sq(B8));
	assert(is_light_sq(C8));
	assert(!is_light_sq(A7));
	assert(is_light_sq(B7));
	assert(!is_light_sq(C7));
	assert(is_light_sq(A6));
	assert(!is_light_sq(B6));
	assert(is_light_sq(C6));
}

void test_is_dark_sq() {
	assert(is_dark_sq(H8));
	assert(!is_dark_sq(G8));
	assert(is_dark_sq(F8));
	assert(!is_dark_sq(H7));
	assert(is_dark_sq(G7));
	assert(!is_dark_sq(F7));
	assert(is_dark_sq(H6));
	assert(!is_dark_sq(G6));
	assert(is_dark_sq(F6));
}

void test_is_diagonal() {
	assert(is_diagonal(A1,B2));
	assert(!is_diagonal(A1,A2));
	assert(is_diagonal(D4,G7));
	assert(is_diagonal(G7,B2));
	assert(!is_diagonal(C2,E2));
	assert(is_diagonal(A8,H1));
}

void test_square() {
	test_rank();
	test_file();
	test_rank_distance();
	test_file_distance();
	test_get_dir();
	test_north();
	test_east();
	test_south();
	test_west();
	test_northeast();
	test_southeast();
	test_southwest();
	test_northwest();
	test_is_light_sq();
	test_is_dark_sq();
	test_is_diagonal();
}

