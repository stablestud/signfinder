#ifdef __WIN32

#include <stdint.h>

#include <windows.h>

#include <Errhandlingapi.h>
#include <Processthreadsapi.h>
#include <Psapi.h>
#include <Handleapi.h>

#include "args.h"
#include "log.h"
#include "page.h"
#include "page_windows.h"
#include "page_windows.def.h"
#include "process.h"
#include "process_windows.h"

static enum PAGE_PROTECTION get_page_prot(const unsigned long pageprot)
{
	enum PAGE_PROTECTION prot;
	switch(pageprot) {
	case 0x1:
		prot = NOACCESS;
		break;
	case 0x2:
		prot = READONLY;
		break;
	case 0x10:
		prot = EXECUTABLE;
		break;
	case 0x20:
		prot = EXECUTABLE_READONLY; 
		break;
	case 0x4:
		prot = READWRITE;
		break;
	case 0x8:
		prot = COPYONWRITE;
		break;
	case 0x40:
		prot = EXECUTABLE_READWRITE;
		break;
	case 0x80:
		prot = EXECUTABLE_COPYONWRITE;
		break;
	case 0x202:
		prot = NOTCACHABLE_READONLY; 
		break;
	case 0x210:
		prot = NOTCACHABLE_EXECUTABLE; 
		break;
	case 0x220:
		prot = NOTCACHABLE_EXECUTABLE_READONLY; 
		break;
	case 0x204:
		prot = NOTCACHABLE_READWRITE; 
		break;
	case 0x208:
		prot = NOTCACHABLE_COPYONWRITE; 
		break;
	case 0x240:
		prot = NOTCACHABLE_EXECUTABLE_READWRITE; 
		break;
	case 0x280:
		prot = NOTCACHABLE_EXECUTABLE_COPYONWRITE; 
		break;
	case 0x102:
		prot = GUARDPAGE_READONLY; 
		break;
	case 0x110:
		prot = GUARDPAGE_EXECUTABLE; 
		break;
	case 0x120:
		prot = GUARDPAGE_EXECUTABLE_READONLY; 
		break;
	case 0x104:
		prot = GUARDPAGE_READWRITE; 
		break;
	case 0x108:
		prot = GUARDPAGE_COPYONWRITE; 
		break;
	case 0x140:
		prot = GUARDPAGE_EXECUTABLE_READWRITE;
		break;
	case 0x180:
		prot = GUARDPAGE_EXECUTABLE_COPYONWRITE;
		break;
	case 0x402:
		prot = WRITECOMBINE_READONLY; 
		break;
	case 0x410:
		prot = WRITECOMBINE_EXECUTABLE; 
		break;
	case 0x420:
		prot = WRITECOMBINE_EXECUTABLE_READONLY; 
		break;
	case 0x404:
		prot = WRITECOMBINE_READWRITE; 
		break;
	case 0x408:
		prot = WRITECOMBINE_COPYONWRITE; 
		break;
	case 0x440:
		prot = WRITECOMBINE_EXECUTABLE_READWRITE;
		break;
	case 0x480:
		prot = WRITECOMBINE_EXECUTABLE_COPYONWRITE;
		break;
	default:
		prot = PROT_INVALID;
		break;
	}
	return prot;
}

const char* get_prot_string(const enum PAGE_PROTECTION prot)
{
	const char* str;
	switch(prot) {
	case NOACCESS:
		str = "NOACCESS";
		break;
	case READONLY:
		str = "READONLY";
		break;
	case EXECUTABLE:
		str = "EXECUTABLE";
		break;
	case EXECUTABLE_READONLY:
		str = "EXECUTABLE_READONLY";
		break;
	case READWRITE:
		str = "READWRITE";
		break;
	case COPYONWRITE:
		str = "COPYONWRITE"; 
		break;
	case EXECUTABLE_READWRITE:
		str = "EXECUTABLE_READWRITE"; 
		break;
	case EXECUTABLE_COPYONWRITE:
		str = "EXECUTABLE_COPYONWRITE"; 
		break;
	case NOTCACHABLE_READONLY:
		str = "NOTCACHABLE_READONLY"; 
		break;
	case NOTCACHABLE_EXECUTABLE:
		str = "NOTCACHABLE_EXECUTABLE"; 
		break;
	case NOTCACHABLE_EXECUTABLE_READONLY:
		str = "NOTCACHABLE_EXECUTABLE_READONLY"; 
		break;
	case NOTCACHABLE_READWRITE:
		str = "NOTCACHABLE_READWRITE"; 
		break;
	case NOTCACHABLE_COPYONWRITE:
		str = "NOTCACHABLE_COPYONWRITE"; 
		break;
	case NOTCACHABLE_EXECUTABLE_READWRITE:
		str = "NOTCACHABLE_EXECUTABLE_READWRITE"; 
		break;
	case NOTCACHABLE_EXECUTABLE_COPYONWRITE:
		str = "NOTCACHABLE_EXECUTABLE_COPYONWRITE"; 
		break;
	case GUARDPAGE_READONLY:
		str = "GUARDPAGE_READONLY"; 
		break;
	case GUARDPAGE_EXECUTABLE:
		str = "GUARDPAGE_EXECUTABLE"; 
		break;
	case GUARDPAGE_EXECUTABLE_READONLY:
		str = "GUARDPAGE_EXECUTABLE_READONLY"; 
		break;
	case GUARDPAGE_READWRITE:
		str = "GUARDPAGE_READWRITE"; 
		break;
	case GUARDPAGE_COPYONWRITE:
		str = "GUARDPAGE_COPYONWRITE"; 
		break;
	case GUARDPAGE_EXECUTABLE_READWRITE:
		str = "GUARDPAGE_EXECUTABLE_READWRITE";
		break;
	case GUARDPAGE_EXECUTABLE_COPYONWRITE:
		str = "GUARDPAGE_EXECUTABLE_COPYONWRITE";
		break;
	case WRITECOMBINE_READONLY:
		str = "WRITECOMBINE_READONLY"; 
		break;
	case WRITECOMBINE_EXECUTABLE:
		str = "WRITECOMBINE_EXECUTABLE"; 
		break;
	case WRITECOMBINE_EXECUTABLE_READONLY:
		str = "WRITECOMBINE_EXECUTABLE_READONLY"; 
		break;
	case WRITECOMBINE_READWRITE:
		str = "WRITECOMBINE_READWRITE"; 
		break;
	case WRITECOMBINE_COPYONWRITE:
		str = "WRITECOMBINE_COPYONWRITE"; 
		break;
	case WRITECOMBINE_EXECUTABLE_READWRITE:
		str = "WRITECOMBINE_EXECUTABLE_READWRITE"; 
		break;
	case WRITECOMBINE_EXECUTABLE_COPYONWRITE:
		str = "WRITECOMBINE_EXECUTABLE_COPYONWRITE"; 
		break;
	case PROT_INVALID:
	default:
		str = "INVALID";
		break;
	}
	return str;
}

static enum PAGE_STATE get_page_state(const unsigned long pagestate)
{
	enum PAGE_STATE state;
	switch(pagestate) {
	case 0x1000:
		state = COMMIT;
		break;
	case 0x10000:
		state = FREE;
		break;
	case 0x2000:
		state = RESERVE;
		break;
	default:
		state = STATE_INVALID;
		break;
	}
	return state;
}

const char* get_state_string(const enum PAGE_STATE pagestate)
{
	char* str;
	switch(pagestate) {
	case COMMIT:
		str = "COMMIT";
		break;
	case FREE:
		str = "FREE";
		break;
	case RESERVE:
		str = "RESERVE";
		break;
	case STATE_INVALID:
	default:
		str = "INVALID";
		break;
	}
	return str;
}

static enum PAGE_TYPE get_page_type(const unsigned long pagetype)
{
	enum PAGE_TYPE type;
	switch(pagetype) {
	case 0x1000000:
		type = IMAGE;
		break;
	case 0x40000:
		type = MAPPED;
		break;
	case 0x20000:
		type = PRIVATE;
		break;
	default:
		type = TYPE_INVALID;
		break;
	}
	return type;
}

const char* get_type_string(const enum PAGE_TYPE pagetype)
{
	char* str;
	switch(pagetype) {
	case IMAGE:
		str = "IMAGE";
		break;
	case MAPPED:
		str = "MAPPED";
		break;
	case PRIVATE:
		str = "PRIVATE";
		break;
	case TYPE_INVALID:
	default:
		str = "INVALID";
		break;
	}
	return str;
}

static struct page_list make_page(const MEMORY_BASIC_INFORMATION pageinfo)
{
	struct page_list page = {
		.base = (uintptr_t)pageinfo.BaseAddress,
		.size = (uintptr_t)pageinfo.RegionSize,
		.prot = get_page_prot(pageinfo.Protect),
		.state = get_page_state(pageinfo.State),
		.type = get_page_type(pageinfo.Type)
	};
	// TODO add GetMappedFileName for .name
	// If using malloc for name, add destroy_page func
	return page;
}

static MEMORY_BASIC_INFORMATION get_pages_at_addr(const HANDLE prochandle, const uintptr_t addr)
{
	MEMORY_BASIC_INFORMATION pageinfo = { 0 };
	if (VirtualQueryEx(prochandle, (void *)addr, (MEMORY_BASIC_INFORMATION *)&pageinfo, sizeof(pageinfo)) == 0) {
		if (GetLastError() != ERROR_INVALID_PARAMETER) {
			LOGFATAL("cannot read process pages", "");
		}
		// After last page, RegionSize is set to zero
		pageinfo.RegionSize = 0UL;
	}
	return pageinfo;
}

static unsigned long get_proc_page_count(const HANDLE prochandle)
{
	const unsigned long psize = get_sys_page_size();
	const unsigned long ppsize = get_proc_meminfo(prochandle).PagefileUsage;
	const unsigned long spcount = get_sys_total_commited_pages();
	const unsigned long pcount = ppsize / psize;
	if (((ppsize % psize) != 0UL) || (pcount >= spcount)) {
		LOGFATAL("cannot read process pages: ", "invalid number of pages");
	}
	return pcount;
}

static unsigned long iterate_pages(struct page_container *const pcont, const HANDLE prochandle)
{
	if (pcont == NULL) {
		LOGFATAL("cannot read process pages: ", "empty page array");
	}
	uintptr_t ptr = 0U;
	unsigned long i = 0UL;
	MEMORY_BASIC_INFORMATION meminfo;
	do {
		meminfo = get_pages_at_addr(prochandle, ptr);
		ptr += meminfo.RegionSize;
		if (meminfo.RegionSize != 0UL) {
			pcont->pagearr[i++] = make_page(meminfo);
		}
	} while(meminfo.RegionSize != 0UL);
	return i;
}

static struct page_container get_pages(const unsigned pid, const struct args options)
{
	struct page_container pagecont = { 0 };
	HANDLE prochandle = get_proc_handle(pid);
	const unsigned long pcount = get_proc_page_count(prochandle);
	pagecont.pagearr = malloc(pcount * sizeof(struct page_list));
	if (pagecont.pagearr == NULL) {
		LOGFATAL("cannot allocate memory for page storage", "");
	}
	pagecont.count = iterate_pages(&pagecont, prochandle);
	if (pagecont.count > pcount) {
		LOGFATAL("not enough space for page read", "");
	}

	CloseHandle(prochandle);
	return pagecont;
}

struct page_container find_pages(const struct proc_info procinfo, const struct args options)
{
	struct page_container pages = get_pages(procinfo.pid, options);
	return pages;
}

int is_readable(const struct page_list *const page)
{
	if (page == NULL) {
		return 0;
	}
	if (page->state != COMMIT) {
		return 0;
	}
	if (page->type == TYPE_INVALID) {
		return 0;
	}
	switch(page->prot) {
	case READONLY:
	case EXECUTABLE:
	case EXECUTABLE_READONLY:
	case READWRITE:
	case COPYONWRITE:
	case EXECUTABLE_READWRITE:
	case EXECUTABLE_COPYONWRITE:
	case NOTCACHABLE_READONLY:
	case NOTCACHABLE_EXECUTABLE:
	case NOTCACHABLE_EXECUTABLE_READONLY:
	case NOTCACHABLE_READWRITE:
	case NOTCACHABLE_COPYONWRITE:
	case NOTCACHABLE_EXECUTABLE_READWRITE:
	case NOTCACHABLE_EXECUTABLE_COPYONWRITE:
	case WRITECOMBINE_READONLY:
	case WRITECOMBINE_EXECUTABLE:
	case WRITECOMBINE_EXECUTABLE_READONLY:
	case WRITECOMBINE_READWRITE:
	case WRITECOMBINE_COPYONWRITE:
	case WRITECOMBINE_EXECUTABLE_READWRITE:
	case WRITECOMBINE_EXECUTABLE_COPYONWRITE:
		break;
	default:
		return 0;
		break;
	}
	if (page->size <= 0UL) {
		return 0;
	}
	return 1;
}

#endif /* __WIN32 */
