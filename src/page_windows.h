#ifdef __WIN32
#ifndef PAGE_WINDOWS_H
#define PAGE_WINDOWS_H

#include "args.h"
#include "process.h"
#include "page_windows.def.h"

struct page_container find_pages(const struct proc_info procinfo, const struct args options);
const char* get_prot_string(const enum PAGE_PROTECTION);
const char* get_state_string(const enum PAGE_STATE);
const char* get_type_string(const enum PAGE_TYPE);
int is_readable(const struct page_list *const page);
void* read_proc_mem(const unsigned pid, const uintptr_t base, const uintptr_t size);

#endif /* PAGE_WINDOWS_H */
#endif /* __WIN32 */
