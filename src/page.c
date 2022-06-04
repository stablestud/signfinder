#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "args.h"
#include "convert.h"
#include "log.h"
#include "page.h"
#include "search.h"

#ifdef __WIN32
	#include "page_windows.h"
#endif /* __WIN32 */

void page_print_result(const struct page_list pageinfo, const struct args options)
{
	char buf[256];
	if (options.verbose == 1) {
#ifdef __WIN32
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), "0x%llx(+0x%llx) - 0x%llx: \"%s\" (%s/%s/%s)", (uintptr_t)pageinfo.base, pageinfo.size, (uintptr_t)pageinfo.base + pageinfo.size, pageinfo.name, get_type_string(pageinfo.type), get_state_string(pageinfo.state), get_prot_string(pageinfo.prot));
#else
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), "0x%llx(+0x%llx) - 0x%llx: \"%s\"", (uintptr_t)pageinfo.base, pageinfo.size, pageinfo.base + pageinfo.size, pageinfo.name);
#endif /* __WIN32 */
		log_println(buf);
	}

	if (pageinfo.hitstore.hits != NULL) {
		print_hits(pageinfo.hitstore);
	}

	if (options.verbose == 1) {
		log_print("Hits: ");
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), "%lu", pageinfo.hitstore.count);
		log_println(buf);
	}
}

void pages_print_results(const struct page_container *const pagecont, const struct args options)
{
	if (pagecont == NULL) {
		if (options.verbose == 1) {
			log_println("No pages");
		}
		return;
	}

	if (pagecont->count == 0UL) {
		if (options.verbose == 1) {
			log_println("No pages");
		}
		return;
	}

	for (unsigned long i = 0UL; i < pagecont->count; i++) {
		page_print_result(pagecont->pagearr[i], options);
	}
}

unsigned long get_total_page_hits(const struct page_container *const pagecont)
{
	if (pagecont == NULL) {
		return 0UL;
	}
	unsigned long hits = 0UL;
	for (unsigned long i = 0UL; i < pagecont->count; i++) {
		hits += pagecont->pagearr[i].hitstore.count;
	}
	return hits;
}

int should_read_page(const struct page_list *const page, const struct args options)
{
	if (page == NULL) {
		return 0;
	}
	if (page->size <= 0) {
		return 0;
	}
	if (is_readable(page) == 0) {
		return 0;
	}
	// TODO Add filter options here
	return 1;
}

struct page_container destroy_page_container(struct page_container *const pageregion)
{
	if (pageregion == NULL) {
		struct page_container aux = { 0 };
		return aux;
	}
	free(pageregion->pagearr);
	pageregion->pagearr = NULL;
	pageregion->count = 0UL;
	return *pageregion;
}

static struct page_container add_page_to_cont(const struct page_container cont, const struct page_list page)
{
	struct page_container ncont = { 0 };
	if (cont.count == 0UL || cont.pagearr == NULL) {
		ncont.pagearr = alloc_copy_mem(&page, sizeof(page));
		ncont.count = 1UL;
	} else {
		ncont.pagearr = realloc_append_mem(cont.pagearr, cont.count * sizeof(page), &page, sizeof(page));
		ncont.count = cont.count + 1UL;
	}
	return ncont;
}

static struct hits search_page_region(const struct args options, const uintptr_t base, const uintptr_t size)
{
	struct hits hit = { 0 };
	const void *const memory = read_proc_mem(options.pid, base, size);
	hit = search_block(memory, size, options.bytes.data, options.bytes.len, base);
	free((void *const)memory);
	return hit;
}

static int is_next_readable(const struct page_container cont, const unsigned long currpos)
{
	if ((currpos + 1UL) >= cont.count) {
		return 0;
	}
	if (is_readable(&cont.pagearr[currpos+1UL]) != 1) {
		return 0;
	}
	return 1;
}

static uintptr_t get_crosspage_size(const uintptr_t bytelen, const struct page_container cont, const unsigned long currpos)
{
	if (is_next_readable(cont, currpos) == 0) {
		return 0U;
	}
	if (bytelen > cont.pagearr[currpos].size) {
		return cont.pagearr[currpos].size + get_crosspage_size(bytelen - cont.pagearr[currpos].size, cont, currpos);
	}
	return bytelen;
}

struct page_container search_pages(const struct args options, struct page_container pages)
{
	struct page_container npages = { 0 };
	uintptr_t base = 0U;
	uintptr_t size = 0U;
	for (unsigned long i = 0UL; i < pages.count; i++) {
		if ((base + size) != pages.pagearr[i].base) {
			char buf[256];
			snprintf(buf, sizeof(buf) / sizeof(buf[0]), "missing pages: 0x%llx - 0x%llx", base + size, pages.pagearr[i].base);
			LOGFATAL("cannot read process pages: ", buf);
		}

		base = pages.pagearr[i].base;
		size = pages.pagearr[i].size;

		if (should_read_page(&pages.pagearr[i], options) == 1) {
			const uintptr_t scansize = size + get_crosspage_size(options.bytes.len, pages, i) - 1U;
			const struct hits pagehits = search_page_region(options, base, scansize);
			if (pagehits.count > 0UL) {
				npages = add_page_to_cont(npages, pages.pagearr[i]);
				npages.pagearr[npages.count - 1UL].hitstore = pagehits;
			}
		}
	}
	return npages;
}
