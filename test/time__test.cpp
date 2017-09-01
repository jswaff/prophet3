#include <cassert>
#include "../src/defs.h"
#include "../src/globals.h"

void test_get_search_time() {
	// should leave 1/10th second on the clock if possible
	assert(get_search_time(100,0)==0);
	assert(get_search_time(99,0)==0);
	assert(get_search_time(25,0)==0);
	assert(get_search_time(-1,3)==0);
	assert(get_search_time(-25,2)==0);

	assert(get_search_time(380,299)==214);
}

void test_time() {
	test_get_search_time();
}
