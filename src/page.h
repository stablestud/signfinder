#ifndef PAGE_H
#define PAGE_H

struct page_list; // To fix circular dependency problems
struct page_container;

#include "args.h"
#include "search.h"

#ifdef __WIN32
	#include "page_windows.def.h"
#elif defined(unix) || defined(__unix__) || defined(__unix)
	#error *NIX not implemented yet
#else
	#error Unsupported OS
#endif

struct page_list { // TODO Rename to page_region
	const char* name;
	uintptr_t base;
	uintptr_t size;
	struct hits hitstore;
#if __WIN32
	enum PAGE_PROTECTION prot;
	enum PAGE_STATE state;
	enum PAGE_TYPE type;
#endif /* __WIN32 */
};

struct page_container {
	unsigned long count;
	struct page_list* pagearr;
};

void pages_print_results(const struct page_container *const pagecont, const struct args options);
void page_print_result(const struct page_list pageinfo, const struct args options);
unsigned long get_total_page_hits(const struct page_container *const pagecont);
struct page_container search_pages(const struct args options, struct page_container pages);
struct page_container destroy_page_container(struct page_container *const pageregion);

#endif /* PAGE_H */
