#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "search.h"

static int compare_mem(const void *const data1, const void *const data2, const uintptr_t len);
static struct hits add_hit(struct hits hit_arr, const uintptr_t ptr);
struct hits destroy_hitlist(struct hits matches);

#include <stdio.h> // FIXME
struct hits search_block(const void *const data, const uintptr_t lendata, const void *const pattern, const uintptr_t lenpatt, const uintptr_t realaddr)
{
	struct hits matches = { 0 };
	const long diff = lendata - lenpatt;
	if (data == NULL || pattern == NULL) {
		return matches;
	}

	if (diff == 0L) {
		if (compare_mem(data, pattern, lenpatt) == 0) {
			return add_hit(matches, realaddr);
		}
	} else if (diff > 0L) {
		for (unsigned long i = 0UL; i < diff + 1L; i++) {
			if (compare_mem((void *const)((uintptr_t)data + i), pattern, lenpatt) == 0) {
				matches = add_hit(matches, realaddr + i);
			}
		}
	}

	return matches;
}

static int compare_mem(const void *const data1, const void *const data2, const uintptr_t len)
{
	// TODO Add '??', '?' search term as ALL matching byte pattern
	return memcmp(data1, data2, len);
}

static struct hits add_hit(struct hits hit_arr, const uintptr_t ptr)
{
	struct hits aux;
	if (hit_arr.count == 0) {
		aux.count = 1;
		aux.hits = malloc(sizeof(uintptr_t) * 1);
		aux.hits[0] = ptr;
	} else {
		aux.count = hit_arr.count + 1;
		aux.hits = malloc(sizeof(uintptr_t) * aux.count);
		memcpy(aux.hits, hit_arr.hits, hit_arr.count * sizeof(uintptr_t));
		aux.hits[aux.count-1] = ptr;
		destroy_hitlist(hit_arr);
	}
	return aux;
}

struct hits merge_hits(struct hits hits1, struct hits hits2)
{
	struct hits aux;
	aux.count = hits1.count + hits2.count;
	aux.hits = malloc(sizeof(uintptr_t) * (hits1.count + hits2.count));
	memcpy(aux.hits, hits1.hits, hits1.count * sizeof(uintptr_t));
	memcpy(aux.hits + hits1.count, hits2.hits, hits2.count * sizeof(uintptr_t));
	destroy_hitlist(hits1);
	destroy_hitlist(hits2);
	return aux;
}

struct hits destroy_hitlist(struct hits matches)
{
	free(matches.hits);
	matches.hits = NULL;
	matches.count = 0;
	return matches;
}

void print_hits(const struct hits matches)
{
	char buf[32];
	for (unsigned i = 0; i < matches.count; i++) {
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), "0x%llx", matches.hits[i]);
		log_println(buf);
	}
}
