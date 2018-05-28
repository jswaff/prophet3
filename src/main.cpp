/*
 * main.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: james
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "defs.h"
#include "globals.h"

char *book_path = 0;
char *tsfile = 0;
int32 tstime = 5; // default to 5 seconds per problem
uint32 hash_size = 0;
uint32 pawn_hash_size = 0;


void print_greeting() {
	print("Hello!  This is the Prophet Chess Engine v3.0 20180526\n");
	print("James Swafford, 2011-2018\n\n");
}

void assert_assumptions() {
	assert(sizeof(int32)==4);
	assert(sizeof(uint32)==4);
	assert(sizeof(int64)==8);
	assert(sizeof(uint64)==8);

	// order of pieces is important in Qsearch when evaluating potentially losing capture
	assert(NO_PIECE==0);
	assert(NO_PIECE < PAWN);
	assert(PAWN < KNIGHT);
	assert(KNIGHT < BISHOP);
	assert(BISHOP < ROOK);
	assert(ROOK < QUEEN);
	assert(QUEEN < KING);
}

bool run_tests() {
	print("THIS IS A DEBUG BUILD.... running tests...\n");
	test_defs();
	test_bitboards();
	test_magic();
	test_draw();
	test_init();
	test_utils();
	test_square();
	test_pos();
	test_attack();
	test_move();
	test_movegen();
	test_next();
	test_apply();
	test_eval();
	test_evalp();
	test_search();
	test_time();
	test_hash();
	test_mvvlva();
	test_extend();
	test_prune();
	test_see();
	print("testing complete.\n");
	reset_pos(&gpos);
	clear_hash_table(&htbl);
	clear_hash_table(&phtbl);

	return true;
}

void process_args(int argc,char *argv[]) {

	for (int i=1;i<argc;i++) {
		char *p = strchr(argv[i],'=');
		if (p) {
			if (!strncmp(argv[i],"-book",5)) {
				book_path = strdup(p+1);
			} else if (!strncmp(argv[i],"-hash",5)) {
				hash_size = atoi(p+1) *1024*1024; // MB
			} else if (!strncmp(argv[i],"-phash",6)) {
				pawn_hash_size = atoi(p+1) *1024*1024; // MB
			} else if (!strncmp(argv[i],"-suite",6)) {
				tsfile = strdup(p+1);
			} else if (!strncmp(argv[i],"-time",5)) {
				tstime = atoi(p+1);
			}
		}
	}
}

int main(int argc,char *argv[]) {
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	init_logfile();

	char input[255];
	print_greeting();
	assert_assumptions();
	init();

	process_args(argc,argv);

	if (hash_size==0) hash_size = default_hash_size;
	print("initializing hash table with %d bytes\n",hash_size);
	init_hash_table(&htbl,hash_size);

	if (pawn_hash_size==0) pawn_hash_size = default_hash_size;
	print("initializing pawn hash table with %d bytes\n",pawn_hash_size);
	init_hash_table(&phtbl,pawn_hash_size);

	assert(run_tests());

	if (tsfile != 0) {
		test_suite(tsfile,tstime);
		exit(0);
	}

	if (book_path != 0) {
		int32 num_moves = open_book_db(book_path);
		print("opened book %s with %d moves.\n",book_path,num_moves);
		assert(test_book());
	}

	while (1) {
		int result=scanf("%s",input);
		if (result==EOF) {
			error("EOF\n");
			exit(1);
		}
		parse_command(input);
	}

}
