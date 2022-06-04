#include <stdlib.h>

#include "args.h"
#include "convert.h"
#include "log.h"
#include "page.h"
#include "process.h"

#ifdef __WIN32
	#include "page_windows.h"
	#include "process_windows.h"
#elif defined(unix) || defined(__unix__) || defined(__unix)
	#error *NIX not implemented yet
#else
	#error Unsupported OS
#endif

struct proc_info destroy_proc(struct proc_info proc);
struct page_list destroy_pagelist(struct page_list pagelist);

static struct proc_info find_process(const unsigned pid)
{
	struct proc_info proc = find_proc(pid);
	if (proc.pid == 0) {
		char buf[24];
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), "%i", pid);
		LOGFATAL("process not found with pid: ", buf);
	}
	return proc;
}

struct proc_info search_process(const struct args options)
{
	struct proc_info proc = find_process(options.pid);
	proc.pages = find_pages(proc, options);
	if (proc.pages.count == 0UL) {
		LOGFATAL("cannot read process pages: ", "no pages");
	}
	proc.pages = search_pages(options, proc.pages);
	return proc;
}

void process_print_result(const struct proc_info proc, const struct args options)
{
	char buf[256];
	if (options.verbose == 1) {
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), "Searching pid %lu: %s", proc.pid, proc.name);
		log_println(buf);
	}
	pages_print_results(&proc.pages, options);
	if (options.verbose == 1) {
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), "Total hits: %lu", get_total_page_hits(&proc.pages));
		log_println(buf);
	}
}

struct proc_info destroy_proc(struct proc_info proc)
{
	if (proc.name != NULL) {
		free((void *)proc.name);
	}
	if (proc.pages.count != 0UL && proc.pages.pagearr != NULL) {
		free(proc.pages.pagearr);
	}
	return proc;
}
