#include "args.h"
#include "file.h"
#include "log.h"
#include "process.h"
#include "search.h"

int main(const int argc, const char *const argv[])
{
	const struct args options = parse_args(argc, argv);

	if (options.verbose != 0) {
		print_bytes(options.bytes);
	}

	if (options.file != NULL) {
		struct hits matches = search_file(options);
		file_print_result(matches, options);
		matches = destroy_hitlist(matches);
	} else {
		struct proc_info proc = search_process(options);
		process_print_result(proc, options);
		proc = destroy_proc(proc);
	}
	return 0;
}
