#ifdef __WIN32
#ifndef PROCESS_WINDOWS_H
#define PROCESS_WINDOWS_H

#include <windows.h>

#include <Sysinfoapi.h>
#include <Handleapi.h>
#include <Psapi.h>

#include "process.h"

enum PROC_BITS {
	BITS_INVALID,
	X86,
	X86_64
};

enum PROC_BITS get_proc_bits(const HANDLE prochandle);
HANDLE get_proc_handle(const unsigned pid);
PERFORMANCE_INFORMATION get_sys_info(void);
PROCESS_MEMORY_COUNTERS get_proc_meminfo(const HANDLE prochandle);
unsigned long get_sys_page_size(void);
unsigned long get_sys_total_commited_pages(void);
struct proc_info find_proc(const unsigned pid);

#endif /* PROCESS_WINDOWS_H */
#endif /* __WIN32 */
