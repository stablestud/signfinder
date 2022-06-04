#ifdef __WIN32

#undef UNICODE

#include <stdint.h>
#include <stdlib.h>

#include <windows.h>

#include <Errhandlingapi.h>
#include <Handleapi.h>
#include <Memoryapi.h>
#include <Processthreadsapi.h>
#include <Psapi.h>
#include <Sysinfoapi.h>
#include <Tlhelp32.h>
#include <Wow64apiset.h>

#include "convert.h"
#include "log.h"
#include "process.h"
#include "process_windows.h"

static HANDLE get_proc_list(void)
{
	HANDLE proc_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (proc_handle == INVALID_HANDLE_VALUE) {
		LOGFATAL("cannot read process list", "");
	}
	return proc_handle;
}

HANDLE get_proc_handle(const unsigned pid)
{
	HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (proc_handle == NULL) {
		LOGFATAL("cannot open process", "");
	}
	return proc_handle;
}

PROCESS_MEMORY_COUNTERS get_proc_meminfo(const HANDLE prochandle)
{
	PROCESS_MEMORY_COUNTERS procmeminfo;
	if (GetProcessMemoryInfo(prochandle, &procmeminfo, sizeof(procmeminfo)) == FALSE) {
		LOGFATAL("cannot read process memory info", "");
	}
	return procmeminfo;
}

enum PROC_BITS get_proc_bits(const HANDLE prochandle)
{
	BOOL bits;
	enum PROC_BITS arch = BITS_INVALID;
	if (IsWow64Process(prochandle, &bits) == 0) {
		LOGFATAL("cannot get process bit width", "");
	}
	switch(bits) {
	case TRUE:
		arch = X86;
		break;
	case FALSE:
		arch = X86_64;
		break;
	default:
		LOGFATAL("cannot get process bit width", "");
		break;
	}
	return arch;
}

static struct proc_info make_proc_info(const LPPROCESSENTRY32 procinfo)
{
	struct proc_info proc = {
		.name = alloc_copy_string(procinfo->szExeFile),
		.pid  = procinfo->th32ProcessID
	};
	return proc;
}

static int iterate_procs(const unsigned pid, const LPPROCESSENTRY32 procstruct, const HANDLE prochandle)
{
	if (Process32Next(prochandle, procstruct) != TRUE) {
		if (GetLastError() != ERROR_NO_MORE_FILES) {
			LOGFATAL("cannot read process list", "");
		}
		return 0;
	}

	if (procstruct->th32ProcessID == pid) {
		return 1;
	} else {
		return iterate_procs(pid, procstruct, prochandle);
	}
}

static struct proc_info find_pid(const unsigned pid, const HANDLE prochandle)
{
	struct proc_info proc = { NULL };
	PROCESSENTRY32 procstruct = {
		.dwSize = sizeof(PROCESSENTRY32)
	};

	if (Process32First(prochandle, &procstruct) != TRUE) {
		LOGFATAL("cannot read process list", "");
	}

	if (procstruct.th32ProcessID != pid) {
		if (iterate_procs(pid, &procstruct, prochandle) != 1) {
			return proc;
		}
	}

	proc = make_proc_info(&procstruct);
	return proc;
}

struct proc_info find_proc(const unsigned pid)
{
	HANDLE prochandle = get_proc_list();
	struct proc_info proc = find_pid(pid, prochandle);
	CloseHandle(prochandle);
	if (proc.pid == 0) {
		return proc;
	}
	return proc;
}

PERFORMANCE_INFORMATION get_sys_info(void)
{
	PERFORMANCE_INFORMATION perfinfo;
	if (GetPerformanceInfo(&perfinfo, sizeof(perfinfo)) == FALSE) {
		LOGFATAL("cannot read system performance info", "");
	}
	return perfinfo;
}

unsigned long get_sys_page_size(void)
{
	return get_sys_info().PageSize;
}

unsigned long get_sys_total_commited_pages(void)
{
	return get_sys_info().CommitTotal;
}

void* read_proc_mem(const unsigned pid, const uintptr_t base, const uintptr_t size)
{
	const HANDLE prochandle = get_proc_handle(pid);
	unsigned long long bread = 0;
	void *const buf = malloc(size);
	if (buf == NULL) {
		LOGFATAL("cannot read process memory: ", "buffer alloc failed");
	}
	if (ReadProcessMemory(prochandle, (void *)base, buf, size, &bread) == 0) {
		LOGFATAL("cannot read process memory: ", "read failed");
	}
	if (bread != size) {
		LOGFATAL("cannot read process memory: ", "short read");
	}
	CloseHandle(prochandle);
	return buf;
}

#endif /* __WIN32 */
