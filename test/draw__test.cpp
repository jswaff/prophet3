/*
 * draw__test.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: James
 */

#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"

void test_is_draw_by_50() {
	position pos;
	reset_pos(&pos);
	assert(!is_draw_by_50(&pos));

	pos.fifty_counter=pos.move_counter=49;
	assert(!is_draw_by_50(&pos));

	pos.fifty_counter=pos.move_counter=50;
	assert(!is_draw_by_50(&pos));

	pos.fifty_counter=pos.move_counter=51;
	assert(!is_draw_by_50(&pos));

	pos.fifty_counter=pos.move_counter=99;
	assert(!is_draw_by_50(&pos));

	pos.fifty_counter=pos.move_counter=100;
	assert(is_draw_by_50(&pos));
}

void test_is_lack_of_mating_material() {
	position pos;

	// any position with a pawn is not drawn
	reset_pos(&pos);
	assert(!is_lack_of_mating_material(&pos));

	// one pawn
	set_pos(&pos,"4k3/8/8/8/8/8/P7/4K3 w - -");
	assert(!is_lack_of_mating_material(&pos));

	// one bishop
	set_pos(&pos,"4k3/8/8/8/8/8/B7/4K3 w - -");
	assert(is_lack_of_mating_material(&pos));

	// one knight
	set_pos(&pos,"4k3/8/8/8/8/8/n7/4K3 w - -");
	assert(is_lack_of_mating_material(&pos));

	// one rook
	set_pos(&pos,"4k3/8/8/8/8/8/r7/4K3 b - -");
	assert(!is_lack_of_mating_material(&pos));

	// one queen
	set_pos(&pos,"4k3/8/8/8/8/8/Q7/4K3 w - -");
	assert(!is_lack_of_mating_material(&pos));

	// just kings
	set_pos(&pos,"4k3/8/8/8/8/8/8/4K3 w - -");
	assert(is_lack_of_mating_material(&pos));

	// two knights, both white's
	set_pos(&pos,"4k3/8/8/8/8/8/NN6/4K3 w - -");
	assert(!is_lack_of_mating_material(&pos));

	// two knights, one white's one black's
	set_pos(&pos,"4k3/8/8/8/8/8/Nn6/4K3 b - -");
	assert(!is_lack_of_mating_material(&pos));

	// two bishops, one white one black opposite colors
	set_pos(&pos,"4k3/8/8/8/8/8/Bb6/4K3 w - -");
	assert(!is_lack_of_mating_material(&pos));

	// two bishops, one white one black same color
	set_pos(&pos,"4k3/8/8/8/8/8/B1b5/4K3 b - -");
	assert(is_lack_of_mating_material(&pos));

	// knight vs bishop
	set_pos(&pos,"4k3/8/8/8/8/8/B1n5/4K3 b - -");
	assert(!is_lack_of_mating_material(&pos));
}

void test_is_draw_by_rep() {
	position pos;

	reset_pos(&pos);
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(PAWN,E2,E4),gundos);
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(KNIGHT,G8,F6),gundos);
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(KNIGHT,G1,F3),gundos);
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(KNIGHT,F6,G8),gundos);
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(KNIGHT,F3,G1),gundos); // still 1 (first has ep square)
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(KNIGHT,G8,F6),gundos); // 2
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(KNIGHT,G1,F3),gundos); // 2
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(KNIGHT,F6,G8),gundos); // 2
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(KNIGHT,F3,G1),gundos); //2
	assert(!is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(KNIGHT,G8,F6),gundos); // 3
	assert(is_draw_by_rep(&pos,gundos));

	apply_move(&pos,to_move(PAWN,D2,D4),gundos);
	assert(!is_draw_by_rep(&pos,gundos));
}

void test_draw() {
	test_is_draw_by_50();
	test_is_lack_of_mating_material();
	test_is_draw_by_rep();
}




