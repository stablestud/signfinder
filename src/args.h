#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#define SF_MISSARGS NULL

struct bytearr {
	uintptr_t len;
	uint8_t* data;
};

struct args {
	unsigned pid;
	char* file;
	int verbose;
	struct bytearr bytes;
};

struct args parse_args(const int argc, const char *const argv[]);

#endif /* ARGS_H */
