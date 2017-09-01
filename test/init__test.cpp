/*
 * init__test.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: James
 */

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"

uint32 hamming_distance(uint64 x,uint64 y) {
	uint32 dist = 0;
	uint64 val = x ^ y;

	while (val != 0L) {
		dist++;
		val &= val - 1;
	}

	assert(dist >= 0 && dist <= 64);

	return dist;
}

void test_hamming_distance() {
	assert(hamming_distance(0,0)==0);
	assert(hamming_distance(0,1)==1);
	assert(hamming_distance(0,3)==2);
	assert(hamming_distance(7,1)==2);

	uint64 max32 = 4294967295UL;
	assert(hamming_distance(0,max32)==32);

	uint64 max64 = 18446744073709551615UL;

	assert(hamming_distance(0,max64)==64);
	assert(hamming_distance(max64,0)==64);

	assert(hamming_distance(max32,max64)==32);
}

void test_zkeys() {

	uint64 keys[851];
	//uint32 hdistances[851][851];

	// putting a large array on the stack was causing a stack overflow
	// using malloc puts it on the heap
	uint32* hdistances = (uint32*)malloc(851*851*sizeof(uint32));
	if (!hdistances) {
		error("could not malloc hdistances!\n");
		exit(1);
	}

	uint32 n = 0;

	uint32 i,j,k;
	for (i=0;i<2;i++) { // color
		for (j=0;j<64;j++) { // squares
			for (k=1;k<=6;k++) {
				keys[n++] = zkeys.pieces[k][i][j];
			}
		}
		keys[n++] = zkeys.ptm[i];
	}

	for (i=0;i<16;i++) { // castling rights
		keys[n++] = zkeys.casting_rights[i];
	}

	for (i=0;i<65;i++) { // ep square
		keys[n++] = zkeys.ep[i];
	}

	assert(n==851);

	// measure the distance between each combination of keys
	// note distance[X][X] == 0 and distance[X][Y] == distance[Y][X]
	for (i=0;i<n;i++) {
		for (j=0;j<n;j++) {
			uint32 hd = hamming_distance(keys[i],keys[j]);
			assert(hd >= 0 && hd <= 64);
			if (i==j) assert(hd==0);
			hdistances[i*n+j] = hd;
		}
	}

	for (i=0;i<n;i++) {
		for (j=0;j<n;j++) {
			assert(i >= 0 && i < n && j >= 0 && j < n);
			assert(hdistances[i*n+j]==hdistances[j*n+i]);
		}
	}

	// create an array of the distances for each combination
	uint32 total_distance = 0;
	uint32 num_distances = 0;
	for (i=0;i<n;i++) {
		for (j=i+1;j<n;j++) {
			assert(hdistances[i*n+j] >= 0 && hdistances[i*n+j] <= 64);
			total_distance += hdistances[i*n+j];
			num_distances++;
		}
	}

	assert(num_distances==361675);
	double mean_distance = (double)total_distance / num_distances;
	//print("mean: %f\n",mean_distance);
	// we would expect, on average, half of the bits to be different from
	// on key to the next.
	assert(mean_distance >= 31 && mean_distance <= 33);

	// get standard deviation
	double sum_dev = 0;
	double min_dev=999.9,max_dev=0.0;
	int dev_cnts[64];
	for (i=0;i<64;i++) dev_cnts[i] = 0;
	for (i=0;i<n;i++) {
		for (j=i+1;j<n;j++) {
			double delta = (double)hdistances[i*n+j] - mean_distance;
			sum_dev += delta * delta;

			if (abs(delta) < min_dev) min_dev = abs(delta);
			if (abs(delta) > max_dev) max_dev = abs(delta);
			dev_cnts[(int)abs(delta)]++;
		}
	}
	double std_dev = sqrt(sum_dev/num_distances);
	/*print("stddev: %f\n",std_dev);
	print("mindev: %f\n",min_dev);
	print("maxdev: %f\n",max_dev);
	for (i=0;i<64;i++) {
		if (dev_cnts[i]>0) {
			print("dev: %d ==> %d\n",i,dev_cnts[i]);
		}
	}*/

	free(hdistances);
}

void test_bb_files() {
	assert(bb_files[FILE_A]==
			(bb_squares[A1] | bb_squares[A2] | bb_squares[A3] | bb_squares[A4]
		   | bb_squares[A5] | bb_squares[A6] | bb_squares[A7] | bb_squares[A8]));
	assert(bb_files[FILE_E]==
			(bb_squares[E1] | bb_squares[E2] | bb_squares[E3] | bb_squares[E4]
		   | bb_squares[E5] | bb_squares[E6] | bb_squares[E7] | bb_squares[E8]));
	assert(bb_files[FILE_H]==
			(bb_squares[H1] | bb_squares[H2] | bb_squares[H3] | bb_squares[H4]
		   | bb_squares[H5] | bb_squares[H6] | bb_squares[H7] | bb_squares[H8]));
}

void test_bb_ranks() {
	assert(bb_ranks[RANK_8]==
			(bb_squares[A8] | bb_squares[B8] | bb_squares[C8] | bb_squares[D8]
		   | bb_squares[E8] | bb_squares[F8] | bb_squares[G8] | bb_squares[H8]));
	assert(bb_ranks[RANK_8]==255);
	assert(bb_ranks[RANK_4]==
			(bb_squares[A4] | bb_squares[B4] | bb_squares[C4] | bb_squares[D4]
		   | bb_squares[E4] | bb_squares[F4] | bb_squares[G4] | bb_squares[H4]));
	assert(bb_ranks[RANK_1]==
			(bb_squares[A1] | bb_squares[B1] | bb_squares[C1] | bb_squares[D1]
		   | bb_squares[E1] | bb_squares[F1] | bb_squares[G1] | bb_squares[H1]));
}

void test_bb_squares() {
	for (int i=0;i<64;i++) {
		assert(bb_squares[i] > 0);
		assert(bb_squares[i] == ((uint64)1) << i);
	}
}

void test_init() {
	test_hamming_distance();
	test_zkeys();
	test_bb_squares();
	test_bb_files();
	test_bb_ranks();
}
