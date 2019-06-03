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

int32 time_remaining = 0;
int32 increment = 0;
int32 max_depth = 0;
int32 max_time = 0;
bool post = true;
bool pondering_enabled = false;
bool volatile pondering = false;
move volatile ponder_move;
bool volatile abort_iterator = false;

position search_pos;
pthread_mutex_t ponder_mutex;

// prototype for thread routine
void* think_helper(void *ptr);

search_stats build_search_stats_dto();
void print_search_summary(int last_depth,search_stats *stats);

move search_moves[MOVE_STACK_SIZE];
undo search_undos[UNDO_STACK_SIZE];

void set_time_remaining(int32 tr) { time_remaining = tr; }
void set_increment(int32 inc) { increment = inc; }
void set_max_depth(int32 md) { max_depth = md; }
void set_max_time(int32 mt) { max_time = mt; }
void set_pondering_enabled(bool enabled) { pondering_enabled = enabled; }
void set_post(bool p) { post = p; }
bool is_pondering() { return pondering; }
move get_ponder_move() { return ponder_move; }

void set_abort_iterator(bool abort) {
    abort_iterator = abort;
    set_abort_search(abort);
}

void set_ponder_mode(bool ponder_mode) {
    pondering = ponder_mode;
    set_analysis_mode(ponder_mode);
}

void calculate_search_times() {
    max_time = get_search_time(time_remaining,increment);
    set_stop_time(get_start_time() + max_time);
    printd("# calculated search time: %d\n",max_time);
}

/**
 * think() is responsible for kicking off the iterative deepening search.
 */
void think() {

    // make a copy of the current position.  note that just because we are
    // operating off a copy of the global position, the global position
    // itself should remain unchanged until our search is complete.
    memcpy(&search_pos,&gpos,sizeof(position));

    // initialize the search's undo stack to what's already been played for draw detection
    memcpy(search_undos,gundos,gpos.move_counter * sizeof(undo));

    set_ponder_mode(false);
    set_abort_iterator(false);

    pthread_create(&think_thread,NULL,think_helper,NULL);
}


void* think_helper(void *ptr) {

    move_line pv = iterate(&search_pos,false);

    // sanity check - the global position shouldn't have changed
    assert(equal_pos(&search_pos,&gpos,true));
    assert(undo_stacks_equal(search_undos,gundos,gpos.move_counter));

    apply_move(&gpos,pv.mv[0],gundos);
    print_move(pv.mv[0]);
    game_status gs = get_game_status();


    // pondering loop.  as long as we guess correctly we'll loop back around.
    // if we don't predict correctly this thread is terminated.
    bool ponder_failure = false;
    char move_buffer[8];
    while (gs==INPROGRESS && pondering_enabled && pv.n > 1 && !ponder_failure && !abort_iterator) {

        pthread_mutex_lock(&ponder_mutex);
        printd("# think_helper acquired lock #1 on ponder_mutex\n");

        if (!abort_iterator) {
            ponder_move = pv.mv[1];
            move_to_str(ponder_move,move_buffer);
            apply_move(&search_pos,pv.mv[0],search_undos); // apply the move just made so we're in sync
            apply_move(&search_pos,ponder_move,search_undos); // apply the predicted move

            printd("### START PONDERING: %s\n",move_buffer);

            set_ponder_mode(true);
            set_abort_search(false);

            pthread_mutex_unlock(&ponder_mutex);
            printd("# think_helper released lock #1 on ponder_mutex\n");

            pv = iterate(&search_pos,false);
            printd("# ponder search terminated\n");

            pthread_mutex_lock(&ponder_mutex);
            printd("# think_helper acquired lock #2 on ponder_mutex\n");

            ponder_move = 0;

            // if we're not in ponder mode, it's because the usermove was correctly predicted and the main
            // thread transitioned the search into "normal mode."
            if (!pondering) {
                assert(equal_pos(&search_pos,&gpos,true));
                assert(undo_stacks_equal(search_undos,gundos,gpos.move_counter));
                apply_move(&gpos,pv.mv[0],gundos);
                print_move(pv.mv[0]);
                gs = get_game_status();
            } else {
                // we're still in ponder mode.  this means the search terminated on its own.
                // in this case just bail out.
                ponder_failure = true;
            }

            set_ponder_mode(false);
        }

        pthread_mutex_unlock(&ponder_mutex);
        printd("# think_helper released lock #2 on ponder_mutex\n");

    }

    // If game is over by rule then print the result
    printd("# exiting search thread\n");

    if (gs != INPROGRESS) {
        print_result(gs);
    }

    return 0;
}

/**
 * iterate over the given position, and return the principal variation.
 *
 * The returned line (PV) is guaranteed to have at least one move.
 */
move_line iterate(position *pos,bool test_suite_mode) {

    move_line pv; pv.n=0;

    if (!test_suite_mode && !pondering && pos->move_counter <= 30) {
        move book_mv = probe_book(pos);
        if (book_mv) {
            printd("# book move\n");
            pv.mv[0] = book_mv;
            pv.n = 1;
            return pv;
        }
    }

    // if just one legal move don't bother searching
    move *endp = gen_legal_moves(search_moves,pos,true,true);
    int num_mvs = num_moves(search_moves,endp,true,true);
    printd("# position has %d move(s)\n",num_mvs);
    if (!test_suite_mode && num_mvs==1) {
        best_at_top(search_moves,endp);
        pv.mv[0] = search_moves[0];
        pv.n = 1;
        return pv;
    }

    clear_hash_table(&htbl);
    clear_hash_table(&phtbl);
    set_start_time(milli_timer());
    if (test_suite_mode) {
        set_stop_time(get_start_time() + max_time);
    } else {
        calculate_search_times();
    }

    search_stats stats = build_search_stats_dto();

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

        score=search(pos,&pv,alpha_bound,beta_bound,depth,&stats,search_moves,search_undos,post);

        if ((score <= alpha_bound || score >= beta_bound) && !is_abort_search()) {
            printd("# research depth %d!  alpha=%d, beta=%d, score=%d\n",depth,alpha_bound,beta_bound,score);
            score=search(pos,&pv,-INF,INF,depth,&stats,search_moves,search_undos,post);
        }

        assert(pv.n > 0);
        if (is_abort_search()) {
            break;
        }

        if (post) {
            print_thinking_output(&pv,depth,score,get_start_time(),stats.nodes);
        }
        memcpy(&stats.last_pv,&pv,sizeof(move_line));

        //char first_line_text[100];
        //line_to_str(&stats.first_line_searched,first_line_text);
        //print("first line: %s\n",first_line_text);

        // if this is a mate score we can stop
        if (abs(score) > CHECKMATE-500) {
            printd("# stopping iterative search because mate found.\n");
            stop_searching = true;
        }

        if (max_depth > 0 && depth >= max_depth) {
            printd("# stopping iterative search on depth.\n");
            stop_searching = true;
        }
        int32 elapsed_time = milli_timer() - get_start_time();
        if (!pondering && !test_suite_mode && elapsed_time > (max_time / 2)) {
            printd("# stopping iterative search because half time expired.\n");
            stop_searching = true;
        }
    } while (!stop_searching);

    assert(pv.n > 0);
    assert(is_line_valid(&pv,pos));

    print_search_summary(depth,&stats);


    return pv;
}

search_stats build_search_stats_dto() {
    search_stats stats;
    stats.nodes=0;
    stats.qnodes=0;
    stats.fail_highs=0;
    stats.fail_lows=0;
    stats.hash_exact_scores=0;
    stats.first_line_searched.n=0;
    stats.prunes=0;
    stats.last_pv.n=0;
    for (int i=0;i<MAX_PLY;i++) {
        stats.last_pv.mv[i] = BADMOVE;
        stats.first_line_searched.mv[i] = BADMOVE;
    }

    return stats;
}

void print_search_summary(int last_depth,search_stats *stats) {
    uint64 total_nodes = stats->nodes + stats->qnodes;

    float interior_pct = stats->nodes / (total_nodes/100.0);
    float qnode_pct = stats->qnodes / (total_nodes/100.0);

    print("\n");
    print("# depth: %d\n",last_depth);
    print("# nodes: %lluk, interior: %lluk (%.2f%%), quiescence: %lluk (%.2f%%)\n",
            total_nodes/1000,stats->nodes/1000,interior_pct,stats->qnodes/1000,qnode_pct);

    int32 search_time_ms = milli_timer() - get_start_time() + 1; // add 1 ms just to avoid div by 0
    float search_time = search_time_ms / 1000.0;
    uint64 nps = (stats->nodes+stats->qnodes) / search_time_ms;
    print("# search time: %.2f seconds, rate: %llu kn/s\n",search_time,nps);

    uint64 hash_hits = htbl.hits;
    uint64 hash_probes = htbl.probes;
    uint64 hash_collisions = htbl.collisions;
    float hash_hit_pct = hash_hits / ((hash_probes+1)/100.0);
    float hash_collision_pct = hash_collisions / ((hash_probes+1)/100.0);
    print("# hash probes: %lluk, hits: %lluk (%.2f%%), collisions: %lluk (%.2f%%)\n",
            hash_probes/1000,
            hash_hits/1000,hash_hit_pct,
            hash_collisions/1000,hash_collision_pct);

    uint64 pawn_hash_hits = phtbl.hits;
    uint64 pawn_hash_probes = phtbl.probes;
    uint64 pawn_hash_collisions = phtbl.collisions;
    float pawn_hash_hit_pct = pawn_hash_hits / ((pawn_hash_probes+1)/100.0);
    float pawn_hash_collision_pct = pawn_hash_collisions / ((pawn_hash_probes+1)/100.0);
    print("# pawn hash probes: %lluk, hits: %lluk (%.2f%%), collisions: %lluk (%.2f%%)\n",
            pawn_hash_probes/1000,
            pawn_hash_hits/1000,pawn_hash_hit_pct,
            pawn_hash_collisions/1000,pawn_hash_collision_pct);

    float fail_high_pct = stats->fail_highs / ((hash_probes+1)/100.0);
    float fail_low_pct = stats->fail_lows / ((hash_probes+1)/100.0);
    float exact_score_pct = stats->hash_exact_scores / ((hash_probes+1)/100.0);

    print("# fail highs: %lluk (%.2f%%), fail lows: %lluk (%.2f%%), exact scores: %lluk (%.2f%%)\n",
            stats->fail_highs,fail_high_pct,
            stats->fail_lows,fail_low_pct,
            stats->hash_exact_scores,exact_score_pct);

    print("# prunes: %llu\n", stats->prunes);
}
