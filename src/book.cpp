/*
 * book.cpp
 *
 *  Created on: Nov 28, 2016
 *      Author: James
 */

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <sqlite3.h>
#include "defs.h"
#include "globals.h"


typedef struct {
    move_freq_pair* mfp;
    int32 n;
} mfpn_type;

int64 bk_pieces[7][2][64];
int64 bk_ptm[2];
int64 bk_castling_rights[4];
int64 bk_ep[64];

const int bk_cr_bq = 0;
const int bk_cr_bk = 1;
const int bk_cr_wq = 2;
const int bk_cr_wk = 3;

static int get_moves_callback(void *data,int argc,char **argv,char **azColName) {

    mfpn_type* mfpn = (mfpn_type*)data;

    int fromsq = atoi(argv[0]);
    int tosq = atoi(argv[1]);
    int freq = atoi(argv[2]);

    for (int i=0;i<mfpn->n;i++) {
        if (get_from_sq(mfpn->mfp[i].mv)==fromsq && get_to_sq(mfpn->mfp[i].mv)==tosq) {
            mfpn->mfp[i].freq += freq;
        }
    }

    return 0;
}

static int load_zkeys_callback(void *data, int argc, char **argv, char **azColName){
    int* num_keys = (int*)data;

    if (argc==1 && argv[0]) {
        int64 key = strtoll(argv[0],(char **)NULL, 10);
        int32 sq_ind = (*num_keys) % 64;
        if (*num_keys < 64) {
            bk_pieces[PAWN][WHITE][sq_ind] = key;
        } else if (*num_keys < 128) {
            bk_pieces[ROOK][WHITE][sq_ind] = key;
        } else if (*num_keys < 192) {
            bk_pieces[KNIGHT][WHITE][sq_ind] = key;
        } else if (*num_keys < 256) {
            bk_pieces[BISHOP][WHITE][sq_ind] = key;
        } else if (*num_keys < 320) {
            bk_pieces[QUEEN][WHITE][sq_ind] = key;
        } else if (*num_keys < 384) {
            bk_pieces[KING][WHITE][sq_ind] = key;
        } else if (*num_keys < 448) {
            bk_pieces[PAWN][BLACK][sq_ind] = key;
        } else if (*num_keys < 512) {
            bk_pieces[ROOK][BLACK][sq_ind] = key;
        } else if (*num_keys < 576) {
            bk_pieces[KNIGHT][BLACK][sq_ind] = key;
        } else if (*num_keys < 640) {
            bk_pieces[BISHOP][BLACK][sq_ind] = key;
        } else if (*num_keys < 704) {
            bk_pieces[QUEEN][BLACK][sq_ind] = key;
        } else if (*num_keys < 768) {
            bk_pieces[KING][BLACK][sq_ind] = key;
        } else if (*num_keys==768) {
            bk_ptm[WHITE] = key;
        } else if (*num_keys==769) {
            bk_ptm[BLACK] = key;
        } else if (*num_keys==770) {
            bk_castling_rights[bk_cr_bq] = key;
        } else if (*num_keys==771) {
            bk_castling_rights[bk_cr_bk] = key;
        } else if (*num_keys==772) {
            bk_castling_rights[bk_cr_wq] = key;
        } else if (*num_keys==773) {
            bk_castling_rights[bk_cr_wk] = key;
        } else if (*num_keys < 838) {
            int32 ep_sq_ind = *num_keys-774;
            bk_ep[ep_sq_ind] = key;
        }

        (*num_keys)++;
    }


    return 0;
}

static int num_moves_callback(void *data, int argc, char **argv, char **azColName){

    int* num_moves = (int*)data;

    if (argc==1 && argv[0]) {
        *num_moves = atoi(argv[0]);
    }

    printd("num moves: %d\n",*num_moves);

    return 0;
}

void load_zkeys() {
    printd("loading zkeys from book...\n");

    char* sql = "select key from zobrist_keys order by id";
    char *err_msg = 0;
    int num_keys = 0;

    int rc = sqlite3_exec(book_db,sql,load_zkeys_callback,(void*)&num_keys,&err_msg);
    if( rc != SQLITE_OK ){
        error("SQL error: %s\n",err_msg);
        sqlite3_free(err_msg);
    }
    printd("loaded %d keys\n",num_keys);
}

int32 open_book_db(char *path) {

    int rc = sqlite3_open(path, &book_db);
    if (rc){
        error("Can't open database: %s\n", sqlite3_errmsg(book_db));
        sqlite3_close(book_db);
        book_db = 0;
        return 0;
    }

    char* sql = "select count(*) from book_moves";
    char *err_msg = 0;
    int num_moves = 0;

    rc = sqlite3_exec(book_db,sql,num_moves_callback,(void*)&num_moves,&err_msg);
    if( rc != SQLITE_OK ){
        error("SQL error: %s\n",err_msg);
        sqlite3_free(err_msg);
    }

    load_zkeys();

    return num_moves;
}

int64 build_book_key(position *p) {
    int64 key = 0;

    for (int32 sq=0;sq<64;sq++) {
        if (p->piece[sq] != NO_PIECE) {
            int32 pc = p->piece[sq];
            int32 ptm = WHITE;
            if (pc < 0) {
                pc = -pc;
                ptm = BLACK;
            }
            key ^= bk_pieces[pc][ptm][sq];
        }
    }

    if (p->castling_rights & 1) {
        key ^= bk_castling_rights[bk_cr_wk];
    }
    if (p->castling_rights & 2) {
        key ^= bk_castling_rights[bk_cr_wq];
    }
    if (p->castling_rights & 4) {
        key ^= bk_castling_rights[bk_cr_bk];
    }
    if (p->castling_rights & 8) {
        key ^= bk_castling_rights[bk_cr_bq];
    }

    if (p->ep_sq != NO_SQUARE) {
        key ^= bk_ep[p->ep_sq];
    }

    key ^= bk_ptm[p->player];

    return key;
}

move probe_book(position *p) {
    move_freq_pair mfp[300];
    int32 num_book_moves = get_book_moves(mfp,p);

    // get the total frequency of all book moves
    int32 total_freq=0;
    char move_buffer[8];
    for (int i=0;i<num_book_moves;i++) {
        if (mfp[i].freq > 0) {
            move_to_str(mfp[i].mv,move_buffer);
            printd("# book move: %s, freq: %d\n",move_buffer,mfp[i].freq);
            total_freq += mfp[i].freq;
        }
    }
    printd("# total freq: %d\n",total_freq);
    if (total_freq==0) {
        return 0;
    }

    // do a weighted random selection
    srand(time(NULL));
    double g = rand() / ((double) RAND_MAX); // [0-1]
    double random_weight = g * total_freq;
    printd("# random weight: %f\n",random_weight);

    int sum_freq = 0;
    move selected_mv = 0;
    for (int i=0;i<num_book_moves;i++) {
        if (mfp[i].freq > 0) {
            sum_freq += mfp[i].freq;
            if (sum_freq >= random_weight) {
                selected_mv = mfp[i].mv;
                move_to_str(selected_mv,move_buffer);
                printd("# selected move %s with sum_freq=%d\n",move_buffer,sum_freq);
                break;
            }
        }
    }

    return selected_mv;
}

int32 get_book_moves(move_freq_pair *mfp,position *p) {
    if (book_db==0) {
        printd("# can't probe - no book database\n");
        return 0;
    }

    printd("# probing book...\n");

    long long key = build_book_key(p);

    char sql[128];
    sprintf(sql,"select fromsq,tosq,frequency,wins,losses,draws from book_moves where key=%lld",key);

    char *err_msg = 0;

    move moves[300],*endp;
    endp = gen_legal_moves(moves,p,true,true);

    int mv_ind=0;
    for (move *mp=moves;mp<endp;mp++) {
        if (*mp) {
            mfp[mv_ind].mv=*mp;
            mfp[mv_ind].freq=0;
            mv_ind++;
        }
    }
    assert(mv_ind<300);

    mfpn_type mfpn;
    mfpn.mfp = &mfp[0];
    mfpn.n = mv_ind;
    int rc = sqlite3_exec(book_db,sql,get_moves_callback,(void*)&mfpn,&err_msg);
    if( rc != SQLITE_OK ){
        error("SQL error: %s\n",err_msg);
        sqlite3_free(err_msg);
    }

    return mv_ind;
}
