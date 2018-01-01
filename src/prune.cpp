/*
 * prune.cpp
 *
 *  Created on: Aug 22, 2016
 *      Author: James
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "globals.h"
#include "defs.h"
#include "eval.h"

bool prune(position *pos,move last_move,bool incheck,bool gives_check,
		int32 extensions,int32 alpha,int32 beta,int32 depth,search_stats *stats) {

	if (depth < 3
		&& alpha < (CHECKMATE-500) && beta < (CHECKMATE-500)
		&& alpha > (-CHECKMATE+500) && beta > (-CHECKMATE+500)
		&& extensions==0 && !incheck && !gives_check
		&& !is_capture(last_move) && get_promopiece(last_move)==NO_PIECE
		&& !(get_piece(last_move)==PAWN && (get_rank(get_to_sq(last_move))==RANK_2 || get_rank(get_to_sq(last_move))==RANK_7))) {

		int32 evalMat = -eval(pos,true,stats);

		return (depth < 2 && (evalMat + pawn_val*2 <= alpha))   // futility pruning
			|| (depth < 3 && (evalMat + pawn_val*5 <= alpha)) ;  // extended futility pruning
	}

	return false;
}



