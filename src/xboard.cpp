/*
 * in.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: james
 */

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sqlite3.h>
#include <unistd.h>
#include "defs.h"
#include "globals.h"


pthread_t ics_thread;
int32 last_input_time;
bool force_mode = true;

void stop_search_thread();

void handle_bk() {
	if (book_db==0) {
		print("\tbook not enabled\n");
	} else {
		move_freq_pair mfp[300];
		int32 num_book_moves = get_book_moves(mfp,&gpos);

		if (num_book_moves > 0) {
			char move_buffer[8];
			for (int i=0;i<num_book_moves;i++) {
				if (mfp[i].freq > 0) {
					move_to_str(mfp[i].mv,move_buffer);
					print("\t%s, freq: %d\n",move_buffer,mfp[i].freq);
				}
			}
		} else {
			print("\tno book moves found\n");
		}
	}
	print("\n"); // blank line required by protocol
}

void handle_drawboard() {
	draw_board(&gpos);
}

void handle_eval() {
	int32 eval_score=eval(&gpos,false);
	print("eval: %d\n",eval_score);
}

/**
 * Set the engine to play neither color.  Stop clocks.  The engine should check that
 * moves received in force mode are legal and made in proper turn, but should not
 * think, ponder, or make moves of its own.
 */
void handle_force() {
	stop_search_thread();
	force_mode=true;
}

void print_thinking_output(move_line* line,int32 depth,int32 score,int32 start_time,uint64 nodes) {
	assert(line);
	assert(line->n > 0);

	char buffer[1024];
	line_to_str(line,buffer);
	int32 time_centis = (milli_timer() - start_time) / 10;
	print("%2d %5d %5d %7llu %s\n",depth,score,time_centis,nodes,buffer);
}

void print_result(game_status gs) {
	if (gs==CHECKMATED) {
		if (gpos.player==WHITE) {
			print("RESULT 0-1 {Black mates}\n");
		} else {
			print("RESULT 1-0 {White mates}\n");
		}
	} else if (gs==STALEMATED) {
		print("RESULT 1/2-1/2 {Stalemate}\n");
	} else if (gs==DRAW_MATERIAL) {
		print("RESULT 1/2-1/2 {Draw by lack of mating material}\n");
	} else if (gs==DRAW_BY_50) {
		print("RESULT 1/2-1/2 {Draw by 50 move rule}\n");
	} else if (gs==DRAW_REP) {
		print("RESULT 1/2-1/2 {Draw by repetition}\n");
	}
}

void stop_search_thread() {
	set_abort_iterator(true);
	pthread_join(think_thread,NULL);
}

void think_and_make_move() {
	// check if game is over before we even get started.
	game_status gs = get_game_status();
	if (gs != INPROGRESS) {
		print_result(gs);
		return;
	}

	think();
}

void handle_easy() {
	set_pondering_enabled(false);
}

/**
 * Leave force mode and set the engine to play the color that is on move.  Associate
 * the engine's clock with the color that is on move, the opponent's clock with the color
 * that is not on move.  Start the engine's clock.  Start thinking and eventually
 * make a move.
 *
 * If the game is over (either before or after a move), the result is printed.  However,
 * we keep "playing on" until WE receive a 'result' from xboard.  This is especially
 * important in the event of claimed draws, since our RESULT might not be received by the
 * GUI until after the user has made a move.
 */
void handle_go() {
	stop_search_thread();
	force_mode=false;
	think_and_make_move();
}

void handle_hard() {
	set_pondering_enabled(true);
}

/**
 * Level.  Currently only using the increment.
 */
void handle_level() {
	char mps[10],base[10];
	if (scanf("%s",mps)==EOF) return;
	if (scanf("%s",base)==EOF) return;
	float f_inc = 0.0;
	if (scanf("%f",&f_inc)==EOF) return;
	int32 my_increment = (int32)(f_inc * 1000); // convert to ms
	print("# increment: %d\n",my_increment);
	set_increment(my_increment);
}


/**
 * memory N
 *
 * This command informs the engine on how much memory it is allowed to use maximally, in MegaBytes.
 * On receipt of this command, the engine should adapt the size of its hash tables accordingly.
 * This command does only fix the total memory use, the engine has to decide for itself (or be
 * configured by the user by other means) how to divide up the available memory between the various
 * tables it wants to use (e.g. main hash, pawn hash, tablebase cache, bitbases). This command will
 * only be sent to engines that have requested it through the memory feature, and only at the start
 * of a game, as the first of the commands to relay engine option settings just before each "new"
 * command.
 */
int32 last_mem_in_mb = 0;
void handle_memory() {
	int32 mem_in_mb;
	if (scanf("%d",&mem_in_mb)==EOF) return;

	print("# received memory command, N=%d\n",mem_in_mb);
	if (mem_in_mb != last_mem_in_mb) {
		free(htbl.tblptr);
		free(phtbl.tblptr);

		uint32 total_hash_size = mem_in_mb * 1024 * 1024; // convert to bytes

		init_hash_table(&htbl,total_hash_size / 2);
		init_hash_table(&phtbl,total_hash_size / 2);

		last_mem_in_mb = mem_in_mb;
	} else {
		print("# memory usage hasn't changed, skipping free/malloc\n");
	}
}

/**
 * If the engine is thinking, it will immediately stop and move.
 */
void handle_move_now() {
	if (!is_pondering()) {
		stop_search_thread();
	}
}

/**
 * Reset the board.  Leave force mode.  Reset clocks and use wall clock for time measurement.
 * Stop clocks.  Do not ponder on this move.  Remove any search depth limit previously set
 * by the <sd> command.
 */
void handle_new() {
	stop_search_thread();
	force_mode = false;
	reset_pos(&gpos);
	set_max_depth(0);
}

/**
 * Turn off thinking/pondering output.
 */
void handle_nopost() {
	set_post(false);
}

/**
 * Read in the opponent's remaining time, in centiseconds.
 */
void handle_otim() {
	int32 opp_time;
	if (scanf("%d",&opp_time)==EOF) return;
}

void handle_perft() {
	draw_board(&gpos);
	int max_depth;
	if (scanf("%d",&max_depth) != EOF) {
		int32 start = milli_timer();
		uint64 nodes=perft_iterate(max_depth);
		int32 end = milli_timer();
		if (end==start) end = start+1;
		print("\n# nodes: %lu\n",nodes);
		print("# elapsed time: %d ms\n",end-start);
		print("# rate: %.2f n/s\n\n",nodes*1000.0/(end-start));
	}
}

void handle_perft_test_fast() {
	perft_test_fast();
}

void handle_perft_test_slow() {
	int depth;
	if (scanf("%d",&depth) != EOF) {
		perft_test_slow(depth);
	}
}

/**
 * Leave force mode and set the engine to play the color that is not on move.
 * Associate the opponent's clock with the color that is on move, the engine's
 * clock with the color that is not on move.  Start the opponent's clock.  If
 * pondering is enabled, the engine should begin pondering.  If the engine later
 * receives a move, it should start thinking and eventually reply.
 */
void handle_playother() {
	// TODO
}

/**
 * Because of the way xboard uses the ping command, we should never see a "ping" command
 * when the engine is on move.  However, if we do, we should not respond with the "pong"
 * until after making the move.  The exception is pondering -- if the engine is pondering
 * it should immediately respond with the pong but continue pondering.
 *
 * The documentation specifically mentions a "?" followed by "ping".  In this case
 * the pong should not be sent until after the move is made.
 *
 */
void handle_ping() {
	if (!is_pondering()) {
		stop_search_thread();
	}
	int n;
	if (scanf("%d",&n) != EOF) {
		print("pong %d\n",n);
	}
}

/**
 * Turn on thinking/pondering output.
 */
void handle_post() {
	set_post(true);
}

void handle_probe_book() {
	move mv = probe_book(&gpos);
	if (mv==0) {
		print("no move found.\n");
	} else {
		print_move(mv);
	}
}

void handle_protover() {
	int version;
	if (scanf("%d",&version) != EOF) {
		if (version < 2) {
			error("protover >= 2 expected.\n");
			exit(0);
		}

		print("feature analyze=0 black=0 colors=0 cores=0 ping=1 draw=0 debug=1 edit=0 ics=1\n");
		print("feature level=0 name=1 nps=0 memory=1 playother=1 pause=0 resume=0 reuse=1 san=0\n");
		print("feature setboard=1 sigint=0 sigterm=0 smp=0 st=0 time=1 usermove=1\n");
		print("feature white=0 variants=\"normal\"\n");
		print("feature done=1\n"); // must be last
	}
}

void handle_quit() {
	stop_search_thread();
	if (book_db != 0) {
		sqlite3_close(book_db);
	}
	free(htbl.tblptr);
	free(phtbl.tblptr);

	print("bye...\n");
	exit(0);
}

/**
 * Receive the engine's rating, followed by the opponent's.
 */
void handle_rating() {
	int my_rating,opp_rating;
	if (scanf("%d",&my_rating)==EOF) return;
	if (scanf("%d",&opp_rating)==EOF) return;
}

/**
 * Retract a move.  Undoes the last two moves and continues playing the same color.
 * Xboard sends this command only when the user is on move.
 */
void handle_remove() {
	stop_search_thread();
	undo_last_move(&gpos,gundos);
	undo_last_move(&gpos,gundos);
}

/**
 * RESULT is either 1-0, 0-1, 1/2-1/2, or *, indicating whether white won, black won,
 * the game was drawn, or the game unfinished.  The COMMENT string is purely human
 * readable.  It is subject to change.
 *
 * We will get a RESULT command even if we already know the game ended.  If we send
 * the RESULT command (e.g. after mating the opponent), we will still get a RESULT
 * command back.
 *
 * We will NOT get a RESULT command if the user stops playing by selecting Reset,
 * Edit Game, Exit, or the like. -- edit 9/8/12 : don't think that's true for Exit
 */
void handle_result() {
	stop_search_thread();
	char result[10],text[100],comment[100];
	if (scanf("%s",result)==EOF) return;
	comment[0]='\0';
	do {
		if (scanf("%s",text)==EOF) return; // the reason the game is over {bla bla}
		if (strlen(comment)>0) strcat(comment," ");
		strcat(comment,text);
	} while (!strchr(text,'}') && strlen(comment) < 50);
	print("# RESULT: %s\n",result);
	print("# COMMENT: %s\n",comment);
}

/**
 * sd DEPTH
 * The engine will limit its thinking to DEPTH ply.  Note the engine will still respect
 * any time control restrictions in place.
 *
 */
void handle_sd() {
	int32 my_max_depth;
	if (scanf("%d",&my_max_depth)==EOF) return;
	print("# MAX DEPTH: %d\n",my_max_depth);
	set_max_depth(my_max_depth);
}

/**
 * Set the board to the FEN.
 *
 * Note: Xboard does not guarantee the FEN to be legal.  If it is not legal, the
 * suggestion is to "tellusererror illegal position" and respond to any attempted
 * move with "illegal move" until the next "new" or "setboard" command.
 */
void handle_setboard() {
	char fen[255];
	if (!gets(fen)) {
		return;
	}
	print("# SETBOARD FEN: %s\n",fen);
	if (!set_pos(&gpos,fen)) {
		print("tellusererror illegal position\n");
	}
	draw_board(&gpos);
}

/**
 * Read in the engine's remaining time, in centiseconds.
 */
void handle_time() {
	int32 my_time_remaining;
	if (scanf("%d",&my_time_remaining)==EOF) return;
	my_time_remaining *= 10; // centiseconds to milliseconds
	print("# TIME REMAINING: %d\n",my_time_remaining);
	set_time_remaining(my_time_remaining);
}

/**
 * Back up one move.  Xboard will never send this without putting the engine in
 * "force" mode first.  We don't have to worry about undoing a move the engine made.
 */
void handle_undo() {
	undo_last_move(&gpos,gundos);
}

void handle_usermove() {
	char mv_text[MAX_CHARS_PER_MOVE];
	if (scanf("%s",mv_text) == EOF) return;
	move mv = str_to_move(mv_text,&gpos);
	if (mv==BADMOVE) {
		print("Illegal move: %s\n",mv_text);
	} else {
		char mv_buffer[MAX_CHARS_PER_MOVE];
		move_to_str(mv,mv_buffer);
		apply_move(&gpos,mv,gundos);

		if (!force_mode) {

			// we might be in a ponder search
			extern pthread_mutex_t ponder_mutex;
			pthread_mutex_lock(&ponder_mutex);
			print("# handle_usermove acquired lock on ponder_mutex\n");

			bool pondering = is_pondering();
			bool predicted = pondering && (mv == get_ponder_move());
			print("# pondering?: %s, predicted?: %s\n",(pondering?"true":"false"),(predicted?"true":"false"));

			// if we are pondering and have correctly predicted the opponent's move, we just transition into
			// a regular search by setting the search time limit and setting ponder=false.
			bool start_new_search;
			if (pondering && predicted) {
				calculate_search_times();
				set_ponder_mode(false);
				start_new_search = false;
			} else {
				set_abort_iterator(true);
				start_new_search = true;
			}

			pthread_mutex_unlock(&ponder_mutex);
			print("# handle_usermove released lock on ponder_mutex\n");

			if (start_new_search) {
				pthread_join(think_thread,NULL);
				think_and_make_move();
			}
		}
	}
}

void no_op() {

}

void no_op_with_arg() {
	char cmd[100];
	if (scanf("%s",cmd)==EOF) { /* just to keep IDE happy */ }
}

struct function_table_entry {
   const char *name;
   void (*func)();
};

struct function_table_entry function_table[] = {
		{"accepted", no_op_with_arg},
		{"analyze", no_op},
		{"bk",handle_bk},
		{"computer", no_op},
		{"drawboard", handle_drawboard},  // not xboard!
		{"db", handle_drawboard},         // not xboard!
		{"draw", no_op},
		{"easy", handle_easy},
		{"eval", handle_eval},            // not xboard!
		{"force", handle_force},
		{"go", handle_go},
		{"hard", handle_hard},
		{"hint", no_op},
		{"ics", no_op_with_arg},
		{"level", handle_level},
		{"memory", handle_memory},
		{"name", no_op_with_arg},
		{"new", handle_new},
		{"nps", no_op},
		{"nopost", handle_nopost},
		{"otim", handle_otim},
		{"perft", handle_perft},          // not xboard!
		{"perft-test-fast",handle_perft_test_fast}, // not xboard!
		{"perft-test-slow",handle_perft_test_slow}, // not xboard!
		{"ping", handle_ping},
		{"playother", handle_playother},
		{"post", handle_post},
		{"probe",handle_probe_book},
		{"protover",handle_protover},
		{"quit", handle_quit},
		{"random", no_op}, // specific to GNU Chess 4 - treat as no-op
		{"rating", handle_rating},
		{"rejected", no_op_with_arg},
		{"remove", handle_remove},
		{"result", handle_result},
		{"setboard", handle_setboard},
		{"sd", handle_sd},
		{"st", no_op},
		{"time", handle_time},
		{"undo", handle_undo},
		{"usermove", handle_usermove},
		{"variant", no_op_with_arg},
		{"xboard", no_op},
		{"?", handle_move_now}
};

void parse_command(const char *cmd) {

	last_input_time = milli_timer();
	print("# RECEIVED: %s\n",cmd);

	int len=sizeof(function_table)/sizeof(function_table_entry);

	for (int i=0;i<len;i++) {
		function_table_entry fte = function_table[i];
		if (strcmp(cmd,fte.name)==0) {
			fte.func();
			return;
		}
	}

	print("Error (unknown command): %s\n",cmd);
}


