/*
 * perft.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: james
 */

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "defs.h"
#include "globals.h"

/*  Initial position
    Depth   Perft(Depth)    Total Nodes
    1   20  20
    2   400     420
    3   8,902   9322
    4   197,281     206,603
    5   4,865,609   5,072,212
    6   119,060,324     124,132,536
    7   3,195,901,860   3,320,034,396
*/

/*  r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -
    Depth   Perft(Depth)    Total Nodes
    1   48  48
    2   2039    2087
    3   97,862  99,949
    4   4,085,603   4,185,552
    5   193,690,690     197,876,242
    6   8,031,647,685
    Note: this exceeds 32 bits  8,229,523,927
*/


/**
 * The term 'perft' was first introduced by Bob Hyatt of Cray Blitz and Crafty fame.
 * It's basically a functional test that walks the move generation tree in depth first
 * fashion, and returning a node (vertex) count when complete.  Tested from a wide
 * range of positions, this gives a very high level of confidence that the move generation
 * and apply functions are working correctly.
 *
 * Another use of 'perft', (and perhaps Hyatt's intention given the name) is to measure
 * the performance of the move gen/apply systems.  By recording the time it takes
 * to complete a perft run and the node count it returns, we have a metric -- nodes per second.
 *
 * Reference: http://chessprogramming.wikispaces.com/Perft
 */
uint64 perft(position *pos,int32 depth) {
    uint64 nodes=0;
    move moves[MAX_MOVES_PER_PLY];

    move *endp = gen_legal_moves(moves,pos,true,true);

    if (depth == 1) {
        return num_moves(moves,endp,true,true);
    }

    for (move* mp=moves;mp<endp;mp++) {
        if (*mp==0) continue; // move purged as illegal
        apply_move(pos,*mp,gundos);
        nodes += perft(pos,depth-1);
        undo_last_move(pos,gundos);
    }

    return nodes;
}

/**
 * perft_iterate
 *
 * Kicks off an iterative perft, from the current position.  First a perft of depth 1
 * is done, then depth 2, up to <max_depth>.
 */
uint64 perft_iterate(int32 max_depth) {
    draw_board(&gpos);
    uint64 total_nodes=0;
    for (int d=1;d<=max_depth;d++) {
        print("perft %d: ",d);
        uint64 nodes=perft(&gpos,d);
        print("%llu\n",nodes);
        total_nodes += nodes;
    }

    return total_nodes;
}


/**
 * Helper function for perft_test.  Looks for a line in <file> containing a substring <line>.
 * The read lines are placed in <buffer>.  The return value indicates whether the sought line
 * was found, and if true the matching line is in <buffer>.
 */
bool get_line(char* buffer,const char* line,FILE* file) {
    bool found=false;

    while (fgets(buffer,255,file) != NULL) {
        if (strstr(buffer,line)) {
            found=true;
            break;
        }
    }

    return found;
}

/**
 * perft_test_slow
 *
 * Kicks off an extensive functional test using perft.  Each position in the perft-random.epd
 * data file is read, with an perft being done on each position to depth <depth>.  The results
 * are compared to the node counts recorded in perft-random.epd.  Failures result in an
 * assertion failure.
 */
void perft_test_slow(int32 depth) {
    print("perft_test_slow, depth=%d\n",depth);
    if (depth < 1 || depth > 6) {
        error("valid perft_test_slow depths are 1-6.\n");
        return;
    }

    static const char filename[] = "../data/perft-random.epd";
    FILE *file = fopen (filename,"r");
    if (file == NULL) {
        perror (filename); // why didn't the file open?
        return;
    }

    char id_line[255],epd_line[255],node_count_line[6][255];

    position pos;
    uint64 total_nps = 0;
    int32 num_probs = 0;
    while (get_line(id_line,"id ",file) && get_line(epd_line,"epd ",file)
        && get_line(node_count_line[0],"perft 1",file) && get_line(node_count_line[1],"perft 2",file)
        && get_line(node_count_line[2],"perft 3",file) && get_line(node_count_line[3],"perft 4",file)
        && get_line(node_count_line[4],"perft 5",file) && get_line(node_count_line[5],"perft 6",file)
        && num_probs < 100)
    {
        print("======================================================================\n");
        print("%s\n",id_line+3);
        print("%s\n",epd_line+4);
        print("======================================================================\n");

        // remove newline from epd
        char *p=strchr(epd_line,'\n');
        if (p != NULL) *p = '\0';

        set_pos(&pos,epd_line+4);
        draw_board(&pos);
        uint64 expected = strtoull(node_count_line[depth-1]+8,NULL,10);

        int32 start_time = milli_timer();
        uint64 actual = perft(&pos,depth);
        int32 elapsed = milli_timer() - start_time + 1; // div by 0

        print("\nexpected (%d): %llu, actual: %llu\n",depth,expected,actual);
        if (actual != expected) {
            exit(1);
        }

        uint64 nps = actual * 1000 / elapsed;
        total_nps += nps;
        print("nps: %lu\n\n",nps);

        num_probs++;
    }
    fclose (file);

    print("average NPS: %lu\n",total_nps / num_probs);
    print("... perft_test_slow complete!\n");
}

uint64 test_case(position *pos,const char *fen,int depth,uint64 expected) {
    set_pos(pos,fen);
    draw_board(pos);
    int32 start_time = milli_timer();
    uint64 actual = perft(pos,depth);
    int32 elapsed = milli_timer() - start_time + 1; // div by 0
    print("\nexpected (%d): %llu, actual: %llu\n",depth,expected,actual);
    print("nps: %lu\n\n",actual*1000/elapsed);
    if (actual != expected) {
        exit(1);
    }
    return actual;
}

void perft_test_fast() {
    print("starting perft_test_fast\n");
    int32 start_time = milli_timer();
    uint64 total_nodes = 0;

    position pos;
    total_nodes += test_case(&pos,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",4,197281);
    total_nodes += test_case(&pos,"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",3,97862);
    total_nodes += test_case(&pos,"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",6,11030083L);
    total_nodes += test_case(&pos,"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",5,15833292L);
    total_nodes += test_case(&pos,"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",4,2103487);
    total_nodes += test_case(&pos,"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",4,3894594L);

    // illegal EP
    total_nodes += test_case(&pos,"3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", 6, 1134888);
    total_nodes += test_case(&pos,"8/8/8/8/k1p4R/8/3P4/3K4 w - - 0 1", 6, 1134888);
    total_nodes += test_case(&pos,"8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1", 6, 1015133);
    total_nodes += test_case(&pos,"8/b2p2k1/8/2P5/8/4K3/8/8 b - - 0 1", 6, 1015133);

    // ep capture checks opponent
    total_nodes += test_case(&pos,"8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", 6, 1440467);
    total_nodes += test_case(&pos,"8/5k2/8/2Pp4/2B5/1K6/8/8 w - d6 0 1", 6, 1440467);

    // short castle gives check
    total_nodes += test_case(&pos,"5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6, 661072);
    total_nodes += test_case(&pos,"4k2r/8/8/8/8/8/8/5K2 b k - 0 1", 6, 661072);

    // long castle gives check
    total_nodes += test_case(&pos,"3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", 6, 803711);
    total_nodes += test_case(&pos,"r3k3/8/8/8/8/8/8/3K4 b q - 0 1", 6, 803711);

    // losing castling rights
    total_nodes += test_case(&pos,"r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", 4, 1274206);
    total_nodes += test_case(&pos,"r3k2r/7b/8/8/8/8/1B4BQ/R3K2R b KQkq - 0 1", 4, 1274206);

    // castling prevented
    total_nodes += test_case(&pos,"r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1", 4, 1720476);
    total_nodes += test_case(&pos,"r3k2r/8/5Q2/8/8/3q4/8/R3K2R w KQkq - 0 1", 4, 1720476);

    // promote out of check
    total_nodes += test_case(&pos,"2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1", 6, 3821001);
    total_nodes += test_case(&pos,"3K4/8/8/8/8/8/4p3/2k2R2 b - - 0 1", 6, 3821001);

    // discovered check
    total_nodes += test_case(&pos,"8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1", 5, 1004658);
    total_nodes += test_case(&pos,"5K2/8/1Q6/2N5/8/1p2k3/8/8 w - - 0 1", 5, 1004658);

    // promote to give check
    total_nodes += test_case(&pos,"4k3/1P6/8/8/8/8/K7/8 w - - 0 1", 6, 217342);
    total_nodes += test_case(&pos,"8/k7/8/8/8/8/1p6/4K3 b - - 0 1", 6, 217342);

    // unpromote to check
    total_nodes += test_case(&pos,"8/P1k5/K7/8/8/8/8/8 w - - 0 1", 6, 92683);
    total_nodes += test_case(&pos,"8/8/8/8/8/k7/p1K5/8 b - - 0 1", 6, 92683);

    // self stalemate
    total_nodes += test_case(&pos,"K1k5/8/P7/8/8/8/8/8 w - - 0 1", 6, 2217);
    total_nodes += test_case(&pos,"8/8/8/8/8/p7/8/k1K5 b - - 0 1", 6, 2217);

    // stalemate checkmate
    total_nodes += test_case(&pos,"8/k1P5/8/1K6/8/8/8/8 w - - 0 1", 7, 567584);
    total_nodes += test_case(&pos,"8/8/8/8/1k6/8/K1p5/8 b - - 0 1", 7, 567584);

    // double check
    total_nodes += test_case(&pos,"8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 4, 23527);
    total_nodes += test_case(&pos,"8/5k2/8/5N2/5Q2/2K5/8/8 w - - 0 1", 4, 23527);

    int32 elapsed = milli_timer() - start_time + 1; // div by 0
    print("\n\naverage nps: %lu\n\n",total_nodes*1000/elapsed);

    print("... perft_test_fast complete!\n");
}
