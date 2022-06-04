#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

struct proc_info; // To fix circular dependency problems

#include "args.h"
#include "page.h"

struct proc_info {
	const char* name;
	unsigned long pid;
	unsigned long pagecount;
	struct page_container pages;
};

struct proc_info search_process(struct args options);
void process_print_result(struct proc_info proc, struct args options);
struct proc_info destroy_proc(struct proc_info proc);

#endif /* PROCESS_H */
