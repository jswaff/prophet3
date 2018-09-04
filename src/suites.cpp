/*
 * suites.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: James
 */

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "defs.h"
#include "globals.h"

int get_ts_moves(position *pos,char *buf,move *m);

void test_suite(char *tsfile,int32 tstime) {
	print("\nprocessing test suite: %s\n",tsfile);
	print("seconds per problem: %d\n",tstime);

	FILE *f = fopen(tsfile,"r");
	if (!f) {
		print("couldn't open test suite file.\n");
		return;
	}

	char *line = 0;
	size_t len = 0;
	ssize_t read;
	int32 num_correct = 0, num_total = 0;

	while((read = getline(&line,&len,f)) != -1) {
		print("\n\n%s",line);

		// this is EPD format, but the first four fields are the same
		// as FEN so we can use our FEN parser.
		char *p = strchr(line,' '); // pieces
		if (!p) { error("parse error getting pieces.\n"); exit(1); }
		p = strchr(p+1,' ');
		if (!p) { error("parse error getting player.\n"); exit(1); }
		p = strchr(p+1,' ');
		if (!p) { error("parse error getting casting rights.\n"); exit(1); }
		p = strchr(p+1,' ');
		if (!p) { error("parse error getting ep square.\n"); exit(1); }

		// create the FEN to give to the parser
		int fen_length = p-line+1;
		char fen[fen_length];
		strncpy(fen,line,fen_length);
		fen[fen_length-1] = 0;
		print("\nfen: %s\n",fen);

		position pos;
		set_pos(&pos,fen);
		draw_board(&pos);

		// now parse the correct move(s)
		p++; // advance past space
		char *p2 = strchr(p,';');
		if (!p2) { error("parse error getting moves.\n"); exit(1); }
		if (*p != 'b' || *(p+1) != 'm' || *(p+2) != ' ') { error("expected bm marker\n"); exit(1); }
		p += 3;
		int moves_length = p2 - p + 1;
		char str_moves[moves_length];
		strncpy(str_moves,p,moves_length);
		str_moves[moves_length-1] = 0;
		print("\nmoves: %s\n",str_moves);

		move ts_moves[5];
		int32 n = get_ts_moves(&pos,str_moves,ts_moves);
		if (n==0) {
			error("couldn't parse any moves!");
			exit(1);
		} else {
			print("parsed moves:\n");
			for (int i=0;i<n;i++) {
				print("\t");
				print_move(ts_moves[i]);
			}
			print("\n");
		}

		clear_hash_table(&htbl);
		clear_hash_table(&phtbl);

		set_abort_iterator(false);
		set_ponder_mode(false);
		set_max_time(tstime*1000);
		move_line pv = iterate(&pos,true);
		num_total++;

		// did we get it right?
		bool correct = false;
		for (int i=0;i<n;i++) {
			if (ts_moves[i]==pv.mv[0]) {
				correct = true;
				break;
			}
		}
		if (correct) {
			print("\nCorrect! - ");
			num_correct++;
		} else {
			print("\nIncorrect - ");
		}
		print("current score: %d / %d\n",num_correct,num_total);
	}

	free(line);
	fclose(f);
}

int get_ts_moves(position *pos,char *buf,move *m) {
	move *m_init = m;
	char *p = strtok(buf," ");

	while (p != NULL && (m-m_init) <= 5) {
		move mv = str_to_move(p,pos);
		if (mv==BADMOVE) {
			error("couldn't parse move.");
			exit(1);
		}
		*m = mv;
		m++;
		p = strtok(NULL," ");
	}

	return m-m_init;
}
