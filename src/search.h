#ifndef SEARCH_H
#define SEARCH_H

#include <stdint.h>

struct hits {
	unsigned long count;
	uintptr_t* hits;
};

struct hits search_block(const void *const data, const uintptr_t lendata, const void *const pattern, const uintptr_t lenpatt, const uintptr_t realaddr);
struct hits merge_hits(struct hits hits1, struct hits hits2);
struct hits destroy_hitlist(struct hits matches);
void print_hits(const struct hits matches);

#endif /* SEARCH_H */
