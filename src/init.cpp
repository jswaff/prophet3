/*
 * init.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: james
 */

#include "defs.h"
#include "globals.h"


void* init_helper(void *ptr) { /* silly hack to init think_thread */ }


void init_zkeys() {

    for (int i=0;i<2;i++) { // color
        for (int j=0;j<64;j++) { // squares
            for (int k=1;k<=6;k++) {
                zkeys.pieces[k][i][j] = random64();
            }
        }

        zkeys.ptm[i] = random64();
    }

    for (int i=0;i<16;i++) { // castling rights
        zkeys.casting_rights[i] = random64();
    }

    for (int i=0;i<65;i++) { // ep square
        zkeys.ep[i] = random64();
    }

}

void init() {
    init_directions();
    init_zkeys();
    init_bitboards();
    init_magics();
    init_evalp_bitboards();
    reset_pos(&gpos);
    pthread_create(&think_thread,NULL,init_helper,NULL);
}
