/*
 * time.cpp
 *
 *  Created on: Feb 1, 2012
 *      Author: james
 */

#include <ctime>
#include <sys/timeb.h>
#include "defs.h"

int32 milli_timer() {
	struct timeb t;
	ftime(&t);
	return (t.time*1000)+t.millitm;
}

void get_time(char* buffer) {
	time_t tim=time(NULL);
	ctime_r(&tim,buffer);
}

int32 get_search_time(int32 remaining_time,int32 increment) {
	if (remaining_time < 0)
		remaining_time = 0;

	int32 search_time = (remaining_time / 25) + increment;

	// try to leave 1/10th of a second for overhead to avoid time losses in very fast games
	search_time -= 100;
	if (search_time < 0) {
		search_time = 0;
	}

	return search_time;
}
