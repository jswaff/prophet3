/*
 * search.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: james
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "globals.h"
#include "defs.h"
#include "eval.h"

int32 search_helper(position *pos,move_line *parent_pv,int32 alpha,int32 beta,int32 depth,int32 ply,
		bool in_pv,bool incheck,bool null_ok,search_stats *stats,move *startp,undo *undo_stack);
void update_parent_pv(move_line *parent_pv,move_line* pv,move best_move);
int32 adjust_final_score_for_mates(position *pos,int32 score,uint32 moves_searched,int32 ply);

move_order_dto build_move_order_dto(move *startp,move pvmv,move hashmv,
		move killer1,move killer2,bool caps_only) {
	assert(get_move_score(&pvmv)==0);
	assert(get_move_score(&hashmv)==0);

	move_order_dto dto;
	dto.startp = dto.currentp = dto.endp = startp;
	dto.pv_move = pvmv;
	dto.hash_move = hashmv;
	dto.killer1 = killer1;
	dto.killer2 = killer2;
	dto.next_stage = PVMOVE;
	dto.caps_only = caps_only;
	return dto;
}

int32 search(position *pos,move_line *parent_pv,int32 alpha,int32 beta,int32 depth,
		search_stats *stats) {

	move* move_stack = (move*)malloc(MOVE_STACK_SIZE * sizeof(move));
	undo* undo_stack = (undo*)malloc(UNDO_STACK_SIZE * sizeof(undo));

	int32 score = search(pos,parent_pv,alpha,beta,depth,stats,move_stack,undo_stack,false);

	free(undo_stack);
	free(move_stack);

	return score;
}

int32 search(position *pos,move_line *parent_pv,int32 alpha,int32 beta,int32 depth,
		search_stats *stats,move *move_stack,undo *undo_stack,bool show_thinking) {

	assert(pos);
	assert(parent_pv);
	assert(stats);
	assert(depth > 0);
	assert(beta > alpha);

	stats->nodes++;

	move_line pv; pv.n=0;

	// search each move
	int32 moves_searched=0;
	uint64 hash_val = get_hash_entry(pos->hash_key,stats);
	move_order_dto mo_dto = build_move_order_dto(move_stack,
			stats->last_pv.n > 0 ? stats->last_pv.mv[0] : 0,
			get_hash_entry_move(hash_val),
			killer1[0],killer2[0],false);
	move *mp;
	while (next(pos,&mp,&mo_dto)) {
		assert(good_move(pos,*mp));
		apply_move(pos,*mp,undo_stack);
		if (in_check(pos,opposite_player(pos->player))) {
			undo_last_move(pos,undo_stack);
			continue;
		}

		if (moves_searched==0) {
			stats->first_line_searched.mv[0] = *mp;
			stats->first_line_searched.n = 1;
		}

		bool gives_check = in_check(pos,pos->player);
		int32 ext = extend(pos,*mp,gives_check);
		int32 score;
		if (moves_searched==0) {
			score = -search_helper(pos,&pv,-beta,-alpha,depth-1+ext,1,
					true,gives_check,false,stats,mo_dto.endp,undo_stack);
		} else {
			/*score = -search_helper(pos,&pv,-(alpha+1),-alpha,depth-1+ext,1,
					false,gives_check,(ext==0),stats,endp,undo_stack);
			if (score > alpha && score < beta) {*/
				score = -search_helper(pos,&pv,-beta,-alpha,depth-1+ext,1,
					false,gives_check,(ext==0),stats,mo_dto.endp,undo_stack);
			//}
		}
		moves_searched++;

		// restore the position
		undo_last_move(pos,undo_stack);

		// If depth==1, we need to continue with the search to ensure we have something to play.
		if (depth > 1 && abort_search) {
			return 0;
		}

		if (score > alpha) {
			if (score >= beta) {
				return beta;
			}
			alpha = score;
			update_parent_pv(parent_pv,&pv,*mp);
			if (show_thinking) {
				print_thinking_output(parent_pv,depth,alpha,stats->start_time,stats->nodes);
			}
		}
	}

	if (moves_searched==0) {
		return in_check(pos,pos->player) ? -CHECKMATE : DRAWSCORE;
	}

	return alpha;
}

/**
 * search_helper is for all nodes except the root.
 */
int32 search_helper(position *pos,move_line *parent_pv,
		int32 alpha,int32 beta,int32 depth,int32 ply,
		bool pv_node,bool incheck,bool null_ok,
		search_stats *stats,move *startp,undo *undo_stack) {

	assert(pos);
	assert(parent_pv);
	assert(ply > 0);
	assert(alpha < beta);
	assert(stats);

	stats->nodes++; // should include leaf nodes
	int32 orig_alpha = alpha;
	parent_pv->n=0;

	// leaf node?
	if (depth < 1 || ply >= MAX_PLY) {
		assert(!incheck);
		return qsearch(pos,alpha,beta,ply,0,incheck,stats,startp,undo_stack);
	}

	// time check
	if (stats->nodes_last_time_check >= 50000) {
		if (milli_timer() >= stats->stop_time) {
			abort_search = true;
			print("# Aborting search depth=%d, ply=%d on time...\n",depth,ply);
			return 0;
		}
		stats->nodes_last_time_check = 0;
	} else {
		stats->nodes_last_time_check++;
	}

	// draw check.  Note: not doing insufficient material (too expensive?)
	if (is_draw_by_rep(pos,undo_stack) || is_draw_by_50(pos)) {
		return DRAWSCORE;
	}

	// check hash table for score and/or move
	uint64 hash_val = get_hash_entry(pos->hash_key,stats);
	if (hash_val != 0 && get_hash_entry_depth(hash_val) >= depth) {
		hash_entry_t hash_entry_type = get_hash_entry_type(hash_val);
		if (hash_entry_type==LOWER_BOUND) {
			if (get_hash_entry_score(hash_val) >= beta) {
				stats->fail_highs++;
				return beta;
			}
		} else if (hash_entry_type==UPPER_BOUND) {
			if (get_hash_entry_score(hash_val) <= alpha) {
				stats->fail_lows++;
				return alpha;
			}
		} else if (hash_entry_type==EXACT_SCORE) {
			stats->hash_exact_scores++;
			return get_hash_entry_score(hash_val);
		}
	}

	// Null Move
	// The idea here is that if my position is so good that, if by giving the opponent
	// an extra turn they can't raise their score to alpha, then when I actually do make
	// a move I will almost surely fail high.
	if (!pv_node && !incheck && null_ok && depth >= 3 && beta < INF && !is_zugzwang(pos)) {
		square_t ep_sq = apply_null_move(pos);
		int nullDepth = depth - 4; // R=3
		if (nullDepth < 1) { nullDepth = 1; } // don't drop into q-search
		int32 null_score = -search_helper(pos,parent_pv,-beta,-beta+1,nullDepth,ply,
				false,false,false,stats,startp,undo_stack);
		undo_null_move(pos,ep_sq);
		if (abort_search) {
			return 0;
		}
		if (null_score >= beta) {
			return beta;
		}
	}

	// get ready to search
	move_line pv; pv.n=0;
	uint32 moves_searched=0,moves_pruned=0;
	move best_mv = 0;
	move_order_dto mo_dto = build_move_order_dto(startp,
			(pv_node && stats->last_pv.n > ply ? stats->last_pv.mv[ply] : 0),
			get_hash_entry_move(hash_val),
			killer1[ply],killer2[ply],false);

	move *mp;
	while (next(pos,&mp,&mo_dto)) {
		assert(good_move(pos,*mp));
		apply_move(pos,*mp,undo_stack);
		if (in_check(pos,opposite_player(pos->player))) {
			undo_last_move(pos,undo_stack);
			continue;
		}

		if (pv_node && moves_searched==0) {
			stats->first_line_searched.mv[ply] = *mp;
			stats->first_line_searched.n = ply+1;
		}
		bool gives_check = in_check(pos,pos->player);
		int32 ext = extend(pos,*mp,gives_check);
		assert(ext >= 0 && ext <= 1);

		int32 score;

		// TODO: refactor so that pruning decision is here.  (So it will need to consider moves_searched.)

		// TODO: after that call a 'reduce()' function that returns an integer
		// Initially that would include the LMR tests.
		// Later add in the razoring tests: https://chessprogramming.wikispaces.com/Razoring

		// call to search should be depth-1+ext-red
		// Try PVS again!


		if (moves_searched==0) {
			score = -search_helper(pos,&pv,-beta,-alpha,depth-1+ext,ply+1,
				pv_node,gives_check,!pv_node,stats,mo_dto.endp,undo_stack);
		} else {

			// if this position is looking unpromising, just skip it
			if (prune(pos,*mp,incheck,gives_check,ext,alpha,beta,depth)) {
				undo_last_move(pos,undo_stack);
				stats->prunes++;
				moves_pruned++;
				continue;
			}

			// if we've searched enough moves and haven't gotten a fail high yet, this is likely
			// a fail-low node.  reduce the remaining moves.  if the score comes back above alpha,
			// then our assumption was wrong and we need to research at full depth.
			if (moves_searched >= 4 && depth >= 3
					&& !pv_node && !incheck && !gives_check && ext==0
					&& !is_capture(*mp) && !get_promopiece(*mp)
					&& *mp != mo_dto.killer1 && *mp != mo_dto.killer2)
			{
				score = -search_helper(pos,&pv,-(alpha+1),-alpha,depth-2,ply+1,
					false,false,true,stats,mo_dto.endp,undo_stack);
			} else {
				score = alpha + 1; // ensure a full depth search
			}

			if (score > alpha) {
				// try a PVS search.  since this is not the PV node, our expectation is that it will not improve
				// alpha.  If it does, then we need to research with a full window to establish the real score.
				//score = -search_helper(pos,&pv,-(alpha+1),-alpha,depth-1+ext,ply+1,
				//	false,gives_check,(ext==0),stats,endp,undo_stack);

				//if (score > alpha && score < beta) {
					score = -search_helper(pos,&pv,-beta,-alpha,depth-1+ext,ply+1,
							false,gives_check,(ext==0),stats,mo_dto.endp,undo_stack);
				//}
			}
		}
		moves_searched++;

		// restore the position
		undo_last_move(pos,undo_stack);

		if (abort_search) {
			return 0;
		}

		if (score > alpha) {
			if (score >= beta) {
				store_hash_entry(&htbl,pos->hash_key,build_hash_val(LOWER_BOUND,depth,score,*mp));
				if (!is_capture(*mp) && !get_promopiece(*mp)) {
					assert(get_move_score(mp)==0);
					killer2[ply] = killer1[ply];
					killer1[ply] = *mp;
				}
				return beta;
			}
			alpha = score;
			best_mv = *mp;
			update_parent_pv(parent_pv,&pv,*mp);
		}
	}

	assert(moves_searched+moves_pruned==num_moves(pos,true,true));
	assert(alpha >= orig_alpha);

	alpha = adjust_final_score_for_mates(pos,alpha,moves_searched,ply);

	hash_entry_t het = best_mv==0 ? UPPER_BOUND : EXACT_SCORE;
	store_hash_entry(&htbl,pos->hash_key,build_hash_val(het,depth,alpha,best_mv));


	return alpha;
}

int32 qsearch(position *pos,int32 alpha,int32 beta,int32 ply,int32 qply,
		bool incheck,search_stats *stats,move *startp,undo *undo_stack) {

	assert(alpha < beta);

	// time check
	if (stats->nodes_last_time_check >= 10000) {
		if (milli_timer() >= stats->stop_time) {
			abort_search = true;
			print("# Aborting qsearch on time...\n");
			return 0;
		}
		stats->nodes_last_time_check = 0;
	} else {
		stats->nodes_last_time_check++;
	}

	// TODO: try hash probe

	int32 stand_pat = eval(pos,false,stats);
	assert(test_eval_symmetry(stand_pat,pos));

	if (stand_pat > alpha) {
		if (stand_pat >= beta) {
			return beta;
		}
		// our static evaluation will serve as the lower bound
		alpha = stand_pat;
	}


	if (ply >= MAX_PLY) {
		return alpha;
	}

	// get ready to search
	uint32 moves_searched=0,moves_pruned=0;
	move_order_dto mo_dto = build_move_order_dto(startp,0,0,0,0,true);

	move *mp;
	//bool just_caps_promos = true; //!incheck || qply>=3;
	while (next(pos,&mp,&mo_dto)) {
		assert(good_move(pos,*mp));
		assert(is_capture(*mp) || get_promopiece(*mp));
		/*if (just_caps_promos && !is_capture(*mp) && !get_promopiece(*mp)) {
			continue;
		}*/

		apply_move(pos,*mp,undo_stack);
		if (in_check(pos,opposite_player(pos->player))) {
			undo_last_move(pos,undo_stack);
			continue;
		}

		// can this capture possibly raise alpha?
		if (!incheck && !get_promopiece(*mp)
				&& (piece_val(get_captured_piece(*mp)) + pawn_val*2 + stand_pat) < alpha) {
			undo_last_move(pos,undo_stack);
			moves_pruned++;
			continue;
		}

		// if not a promising capture just skip
		if (!incheck && !get_promopiece(*mp)
				&& piece_val(get_piece(*mp)) > piece_val(get_captured_piece(*mp))
				&& see(pos,*mp) < 0) {
			undo_last_move(pos,undo_stack);
			moves_pruned++;
			continue;
		}

		bool gives_check = false; // in_check(pos,pos->player);
		stats->qnodes++; // avoids double counting leaf nodes
		int32 score = -qsearch(pos,-beta,-alpha,ply+1,qply+1,gives_check,stats,mo_dto.endp,undo_stack);
		moves_searched++;

		// restore the position
		undo_last_move(pos,undo_stack);

		if (abort_search) {
			return 0;
		}

		if (score > alpha) {
			if (score >= beta) {
				return beta;
			}
			alpha = score;
		}
	}

	assert(moves_searched+moves_pruned==num_moves(pos,true,false));

	return alpha;
}

void update_parent_pv(move_line *parent_pv,move_line* pv,move best_move) {
	assert(parent_pv);
	assert(pv);
	assert(best_move != BADMOVE);

	set_move_score(&best_move,0);

	parent_pv->mv[0] = best_move;
	if (pv->n > 0) {
		memcpy(parent_pv->mv+1,pv->mv,pv->n * sizeof(move));
		parent_pv->n = pv->n + 1;
	} else {
		parent_pv->n = 1;
	}
}

int32 adjust_final_score_for_mates(position *pos,int32 score,uint32 moves_searched,int32 ply) {
	int32 adj_score = score;

	if (moves_searched==0) {
		if (in_check(pos,pos->player)) {
			adj_score = -(CHECKMATE-ply);
		} else {
			adj_score = DRAWSCORE;
		}
	}

	return adj_score;
}

