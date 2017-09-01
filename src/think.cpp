/*
 * think.cpp
 *
 *  Created on: Dec 28, 2011
 *      Author: james
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "defs.h"
#include "globals.h"
#include "eval.h"

typedef struct {
	int32 max_depth;
	int32 remaining_time;
	int32 increment;
	position search_pos;
	bool show_thinking;
} think_data;

// prototype for thread routine
void* think_helper(void *ptr);
think_data td;

search_stats build_search_stats_dto(int32 max_search_time);
void print_search_summary(search_stats *stats);

move search_moves[MOVE_STACK_SIZE];
undo search_undos[UNDO_STACK_SIZE];

/**
 * think() is responsible for kicking off the iterative deepening search.
 */
void think(int32 max_depth,int32 remaining_time,int32 increment,bool show_thinking) {
	td.max_depth = max_depth;
	td.remaining_time = remaining_time;
	td.increment = increment;
	td.show_thinking = show_thinking;

	// make a copy of the current position.  note that just because we are
	// operating off a copy of the global position, the global position
	// itself should remain unchanged until our search is complete.
	memcpy(&td.search_pos,&gpos,sizeof(position));

	// initialize the search's undo stack to what's already been played
	// for draw detection
	memcpy(search_undos,gundos,gpos.move_counter * sizeof(undo));

	abort_search = false;
	clear_hash_table();
	pthread_create(&think_thread,NULL,think_helper,NULL);
}


void* think_helper(void *ptr) {
	// figure out how much time we want to spend searching
	int32 search_time = get_search_time(td.remaining_time,td.increment);
	print("# time remaining in ms: %d, inc=%d, search_time=%d\n",td.remaining_time,td.increment,search_time);

	move_line pv = iterate(&td.search_pos,search_time,td.max_depth,true,td.show_thinking);

	// sanity check - the global position shouldn't have changed
	assert(equal_pos(&td.search_pos,&gpos,true));

	apply_move(&gpos,pv.mv[0],gundos);
	print_move(pv.mv[0]);
	game_status gs = get_game_status();

	// If game is over by rule then print the result
	if (gs != INPROGRESS) {
		print_result(gs);
	}

	return 0;
}

/**
 * iterate over the given position, and return the principal variation.
 * max_search_time is the time to search in milliseconds.  If 0 it is ignored.
 * max_search_depth is the depth to search.  If 0 it is ignored.
 *
 * The returned line (PV) is guaranteed to have at least one move.
 *
 * abort_search should be set before calling this method.
 */
move_line iterate(position *pos,int32 max_search_time,int32 max_search_depth,
		bool allow_early_stop,bool show_thinking) {

	move_line pv; pv.n=0;

	if (pos->move_counter <= 30) {
		move book_mv = probe_book(pos);
		if (book_mv) {
			pv.mv[0] = book_mv;
			pv.n = 1;
			return pv;
		}
	}

	// if just one legal move don't bother searching
	move *endp = gen_legal_moves(search_moves,pos,true,true);
	int num_mvs = num_moves(search_moves,endp,true,true);
	print("# position has %d move(s)\n",num_mvs);
	if (num_mvs==1) {
		best_at_top(search_moves,endp);
		pv.mv[0] = search_moves[0];
		pv.n = 1;
		return pv;
	}

	search_stats stats = build_search_stats_dto(max_search_time);
	int depth = 0;
	bool stop_searching = false;

	int32 score = 0;
	do {
		++depth;

		// aspiration windows
		int32 alpha_bound = -INF;
		int32 beta_bound = INF;
		if (depth > 2) {
			alpha_bound = score - (pawn_val / 3);
			beta_bound = score + (pawn_val / 3);
		}

		score=search(pos,&pv,alpha_bound,beta_bound,depth,&stats,search_moves,search_undos,show_thinking);

		if ((score <= alpha_bound || score >= beta_bound) && !abort_search) {
			print("# research depth %d!  alpha=%d, beta=%d, score=%d\n",depth,alpha_bound,beta_bound,score);
			score=search(pos,&pv,-INF,INF,depth,&stats,search_moves,search_undos,show_thinking);
		}

		assert(pv.n > 0);
		if (abort_search) {
			break;
		}

		if (show_thinking) {
			print_thinking_output(&pv,depth,score,stats.start_time,stats.nodes);
		}
		memcpy(&stats.last_pv,&pv,sizeof(move_line));

		//char first_line_text[100];
		//line_to_str(&stats.first_line_searched,first_line_text);
		//print("first line: %s\n",first_line_text);

		// if this is a mate score we can stop
		if (abs(score) > CHECKMATE-500) {
			stop_searching = true;
		}

		if (max_search_depth > 0 && depth >= max_search_depth) {
			stop_searching = true;
		}
		int32 elapsed_time = milli_timer() - stats.start_time;
		if (allow_early_stop && elapsed_time > (max_search_time / 2)) {
			stop_searching = true;
		}
	} while (!stop_searching);

	assert(pv.n > 0);
	assert(is_line_valid(&pv,pos));

	print_search_summary(&stats);


	return pv;
}

search_stats build_search_stats_dto(int32 max_search_time) {
	search_stats stats;
	stats.start_time = milli_timer();
	stats.stop_time = stats.start_time + max_search_time;
	stats.nodes=0;
	stats.qnodes=0;
	stats.hash_probes=0;
	stats.hash_hits=0;
	stats.hash_collisions=0;
	stats.fail_highs=0;
	stats.fail_lows=0;
	stats.hash_exact_scores=0;
	stats.nodes_last_time_check=0;
	stats.first_line_searched.n=0;
	stats.prunes=0;
	stats.last_pv.n=0;
	for (int i=0;i<MAX_PLY;i++) {
		stats.last_pv.mv[i] = BADMOVE;
		stats.first_line_searched.mv[i] = BADMOVE;
	}

	return stats;
}

void print_search_summary(search_stats *stats) {
	uint64 total_nodes = stats->nodes + stats->qnodes;

	float interior_pct = stats->nodes / (total_nodes/100.0);
	float qnode_pct = stats->qnodes / (total_nodes/100.0);

	print("\n");
	print("# nodes: %lluk, interior: %lluk (%.2f%%), quiescence: %lluk (%.2f%%)\n",
			total_nodes/1000,stats->nodes/1000,interior_pct,stats->qnodes/1000,qnode_pct);

	int32 search_time_ms = milli_timer() - stats->start_time + 1; // add 1 ms just to avoid div by 0
	float search_time = search_time_ms / 1000.0;
	uint64 nps = (stats->nodes+stats->qnodes) / search_time_ms;
	print("# search time: %.2f seconds, rate: %llu kn/s\n",search_time,nps);

	float hash_hit_pct = stats->hash_hits / ((stats->hash_probes+1)/100.0);
	float hash_collision_pct = stats->hash_collisions / ((stats->hash_probes+1)/100.0);
	print("# hash probes: %lluk, hits: %lluk (%.2f%%), collisions: %lluk (%.2f%%)\n",
			stats->hash_probes/1000,
			stats->hash_hits/1000,hash_hit_pct,
			stats->hash_collisions/1000,hash_collision_pct);

	float fail_high_pct = stats->fail_highs / ((stats->hash_probes+1)/100.0);
	float fail_low_pct = stats->fail_lows / ((stats->hash_probes+1)/100.0);
	float exact_score_pct = stats->hash_exact_scores / ((stats->hash_probes+1)/100.0);

	print("# fail highs: %lluk (%.2f%%), fail lows: %lluk (%.2f%%), exact scores: %lluk (%.2f%%)\n",
			stats->fail_highs,fail_high_pct,
			stats->fail_lows,fail_low_pct,
			stats->hash_exact_scores,exact_score_pct);

	print("# prunes: %llu\n", stats->prunes);
}
