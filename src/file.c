#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "file.h"
#include "log.h"
#include "search.h"

static FILE* open_file(const char path[]);
static unsigned calc_ucont_size(const unsigned len);

struct hits search_file(const struct args options)
{
	FILE* file = open_file(options.file);

	const unsigned buflen = calc_ucont_size(options.bytes.len);
	void* buf = malloc(buflen);
	if (buf == NULL) {
		LOGFATAL("cannot allocate memory", "");
	}

	uintptr_t rcount;
	intptr_t fpos = 0;
	struct hits matches = { 0 };

	do {
		rcount = fread(buf, 1, buflen, file);
		if (ferror(file) || errno)
			LOGFATAL("cannot read file: ", strerror(errno));
		struct hits aux = search_block(buf, rcount, options.bytes.data, options.bytes.len, fpos);
		matches = merge_hits(matches, aux);
		if (rcount == buflen) {
			fpos = ftell(file);
			if (fpos == -1 && errno != 0)
				LOGFATAL("cannot read file pos: ", strerror(errno));
			fpos = (fpos - options.bytes.len) + 1;
			if (fseek(file, fpos, SEEK_SET) != 0)
				LOGFATAL("cannot seek in file: ", strerror(errno));
		}
	} while(!feof(file) && !ferror(file) && !errno);

	if (ferror(file) || errno)
		LOGFATAL("cannot read file: ", strerror(errno));

	free(buf);
	fclose(file);
	return matches;
}

static unsigned calc_ucont_size(const unsigned len)
{
	if (len >= (UINT_MAX / 4 - 256)) {
		return len * 2UL;
	} else {
		return len * 4UL + 256UL;
	}
}	

static FILE* open_file(const char path[])
{
	FILE* file = fopen(path, "rb");
	if (file == NULL || errno) {
		LOGFATAL("cannot open file: ", strerror(errno));
	}
	return file;
}

void file_print_result(struct hits matches, struct args options)
{
	if (options.verbose == 1) {
		log_print(options.file);
		log_println(":");
	}

	print_hits(matches);

	if (options.verbose == 1) {
		char buf[32];
		log_print("Hits: ");
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), "%lu", matches.count);
		log_println(buf);
	}
}
