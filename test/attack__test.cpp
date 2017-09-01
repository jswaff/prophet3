/*
 * attack__test.cpp
 *
 *  Created on: Dec 17, 2011
 *      Author: james
 */

#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"

void test_attacked_by_knight() {
	position pos;
	set_pos(&pos,"3k4/8/8/3n4/3N4/8/8/3K4 w - - 0 1");

	uint64 white_map=0,black_map=0;
	for (int32 sq=A8;sq<=H1;sq++) {
		if (attacked_by_knight(&pos,(square_t)sq,WHITE)) {
			white_map |= bb_squares[sq];
		}

		if (attacked_by_knight(&pos,(square_t)sq,BLACK)) {
			black_map |= bb_squares[sq];
		}
	}

	assert(white_map==(bb_squares[C6] | bb_squares[E6] | bb_squares[F5]
	                 | bb_squares[F3] | bb_squares[E2] | bb_squares[C2]
	                 | bb_squares[B3] | bb_squares[B5]));


	assert(black_map==(bb_squares[C7] | bb_squares[E7] | bb_squares[F6]
	                 | bb_squares[F4] | bb_squares[E3] | bb_squares[C3]
	                 | bb_squares[B4] | bb_squares[B6]));


}

void test_attacked_by_king() {
	position pos;
	set_pos(&pos,"3k4/8/8/3n4/3K4/8/8/8 w - - 0 1");

	uint64 white_map=0,black_map=0;
	for (int32 sq=A8;sq<=H1;sq++) {
		if (attacked_by_king(&pos,(square_t)sq,WHITE)) {
			white_map |= bb_squares[sq];
		}

		if (attacked_by_king(&pos,(square_t)sq,BLACK)) {
			black_map |= bb_squares[sq];
		}
	}
	assert(white_map==(bb_squares[C5] | bb_squares[D5] | bb_squares[E5]
	                 | bb_squares[C4] | bb_squares[E4] | bb_squares[C3]
	                 | bb_squares[D3] | bb_squares[E3]));


	assert(black_map==(bb_squares[C8] | bb_squares[E8] | bb_squares[C7]
	                 | bb_squares[D7] | bb_squares[E7]));
}

void test_attacked_by_pawn() {
	position pos;
	set_pos(&pos,"3k4/8/8/3p4/3P3P/8/8/3K4 w - - 0 1");

	for (int32 sq=A8;sq<=H1;sq++) {
		assert(attacked_by_pawn(&pos,(square_t)sq,WHITE)==
				(sq==C5 || sq==E5 || sq==G5));

		assert(attacked_by_pawn(&pos,(square_t)sq,BLACK)==
				(sq==C4 || sq==E4));
	}
}

void test_attacked_by_rook() {
	position pos;
	set_pos(&pos,"8/1r3k2/8/3p4/3P3P/8/1P6/3K4 b - - 0 1");

	uint64 white_map=0,black_map=0;
	for (int32 sq=A8;sq<=H1;sq++) {
		if (attacked_by_rook(&pos,(square_t)sq,WHITE)) {
			white_map |= bb_squares[sq];
		}

		if (attacked_by_rook(&pos,(square_t)sq,BLACK)) {
			black_map |= bb_squares[sq];
		}
	}

	assert(white_map==0);
	assert(black_map==(bb_squares[B8] | bb_squares[A7] | bb_squares[C7]
	        | bb_squares[D7] | bb_squares[E7] | bb_squares[F7]
	        | bb_squares[B6] | bb_squares[B5] | bb_squares[B4]
	        | bb_squares[B3] | bb_squares[B2]));
}

void test_attacked_by_bishop() {
	position pos;
	set_pos(&pos,"8/1r3k2/8/3bB3/8/8/8/3K4 b - - 0 1");

	uint64 white_map=0,black_map=0;
	for (int32 sq=A8;sq<=H1;sq++) {
		if (attacked_by_bishop(&pos,(square_t)sq,WHITE)) {
			white_map |= bb_squares[sq];
		}

		if (attacked_by_bishop(&pos,(square_t)sq,BLACK)) {
			black_map |= bb_squares[sq];
		}
	}

	assert(white_map==(bb_squares[D6] | bb_squares[C7] | bb_squares[B8]
	        | bb_squares[F6] | bb_squares[G7] | bb_squares[H8]
	        | bb_squares[D4] | bb_squares[C3] | bb_squares[B2] | bb_squares[A1]
	        | bb_squares[F4] | bb_squares[G3] | bb_squares[H2]));

	assert(black_map==(bb_squares[C6] | bb_squares[B7]
	        | bb_squares[E6] | bb_squares[F7]
	        | bb_squares[C4] | bb_squares[B3] | bb_squares[A2]
	        | bb_squares[E4] | bb_squares[F3] | bb_squares[G2] | bb_squares[H1]));
}

void test_attacked_by_queen() {
	position pos;
	set_pos(&pos,"8/1r3k2/8/3qQ3/8/8/8/3K4 b - - 0 1");

	uint64 white_map=0,black_map=0;
	for (int32 sq=A8;sq<=H1;sq++) {
		if (attacked_by_queen(&pos,(square_t)sq,WHITE)) {
			white_map |= bb_squares[sq];
		}

		if (attacked_by_queen(&pos,(square_t)sq,BLACK)) {
			black_map |= bb_squares[sq];
		}
	}

	assert(white_map==(bb_squares[D6] | bb_squares[C7] | bb_squares[B8]
	        | bb_squares[F6] | bb_squares[G7] | bb_squares[H8]
	        | bb_squares[D4] | bb_squares[C3] | bb_squares[B2] | bb_squares[A1]
	        | bb_squares[F4] | bb_squares[G3] | bb_squares[H2]
	        | bb_squares[E6] | bb_squares[E7] | bb_squares[E8]
	        | bb_squares[E4] | bb_squares[E3] | bb_squares[E2] | bb_squares[E1]
	        | bb_squares[D5] | bb_squares[F5] | bb_squares[G5] | bb_squares[H5] ));

	assert(black_map==(bb_squares[C6] | bb_squares[B7]
	        | bb_squares[E6] | bb_squares[F7]
	        | bb_squares[C4] | bb_squares[B3] | bb_squares[A2]
	        | bb_squares[E4] | bb_squares[F3] | bb_squares[G2] | bb_squares[H1]
	        | bb_squares[C5] | bb_squares[B5] | bb_squares[A5] | bb_squares[E5]
	        | bb_squares[D6] | bb_squares[D7] | bb_squares[D8]
	        | bb_squares[D4] | bb_squares[D3] | bb_squares[D2] | bb_squares[D1]));
}

void test_get_attackers() {
	position pos;
	set_pos(&pos,"1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - -");

	uint64 attackers = get_attackers(&pos,E5,WHITE);

	assert(attackers & bb_squares[D3]);
	assert(attackers & bb_squares[E2]);
	assert(get_num_bits(attackers)==2);
}

void test_get_attackers2() {
	position pos;
	set_pos(&pos,"1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - -");

	uint64 attackers = get_attackers(&pos,E5,BLACK);

	assert(attackers & bb_squares[D7]);
	assert(attackers & bb_squares[F6]);
	assert(get_num_bits(attackers)==2);
}

void test_get_attackers3() {
	position pos;
	set_pos(&pos,"8/p1k5/1p6/8/3b4/1Q6/8/7K w - -");

	uint64 attackers = get_attackers(&pos,B6,WHITE);

	assert(attackers & bb_squares[B3]);
	assert(get_num_bits(attackers)==1);
}

void test_get_attackers4() {
	position pos;
	set_pos(&pos,"8/p1k5/1p6/8/3b4/1Q6/8/7K w - -");

	uint64 attackers = get_attackers(&pos,B6,BLACK);

	assert(attackers & bb_squares[C7]);
	assert(attackers & bb_squares[A7]);
	assert(attackers & bb_squares[D4]);
	assert(get_num_bits(attackers)==3);
}

void test_attack() {
	test_attacked_by_knight();
	test_attacked_by_king();
	test_attacked_by_pawn();
	test_attacked_by_rook();
	test_attacked_by_bishop();
	test_attacked_by_queen();
	test_get_attackers();
	test_get_attackers2();
	test_get_attackers3();
	test_get_attackers4();
}

