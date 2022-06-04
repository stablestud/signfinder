#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "convert.h"
#include "log.h"

enum loop_state {
	LOOP_FIRST,
	LOOP_OPT,
	LOOP_LONG_OPT,
	LOOP_SHORT_OPT,
	LOOP_OPT_LONG_F,
	LOOP_OPT_LONG_FI,
	LOOP_OPT_LONG_FIL,
	LOOP_OPT_LONG_FILE,
	LOOP_OPT_LONG_H,
	LOOP_OPT_LONG_HE,
	LOOP_OPT_LONG_HEL,
	LOOP_OPT_LONG_HELP,
	LOOP_OPT_LONG_P,
	LOOP_OPT_LONG_PI,
	LOOP_OPT_LONG_PID,
	LOOP_OPT_LONG_V,
	LOOP_OPT_LONG_VE,
	LOOP_OPT_LONG_VER,
	LOOP_OPT_LONG_VERB,
	LOOP_OPT_LONG_VERBO,
	LOOP_OPT_LONG_VERBOS,
	LOOP_OPT_LONG_VERBOSE,
	LOOP_OPT_SHORT_F,
	LOOP_OPT_SHORT_H,
	LOOP_OPT_SHORT_P,
	LOOP_OPT_SHORT_V,
	LOOP_READ,
	LOOP_INVALID_OPT
};

enum arg_type {
	ARG_UNKNWN,
	ARG_VALUE,
	ARG_PID,
	ARG_PID_VALUE,
	ARG_FILE,
	ARG_FILE_VALUE,
	ARG_VERBOSE,
	ARG_HELP,
	ARG_EOAS,		// -- end of arguments
	ARG_INVALID
};

static enum arg_type get_arg_type(const char arg[]);
static unsigned arg_pid(const char *const strpid);
static char* arg_file(const char *const file);
static struct bytearr arg_value(const char *const value, struct bytearr bytes);


struct args parse_args(const int argc, const char *const argv[])
{
	struct args options = { 0 };

	if (argc == 1) {
		LOGFATAL("missing arguments", "");
	}

	enum arg_type thistype = ARG_UNKNWN;
	enum arg_type nexttype = ARG_UNKNWN;
	for (unsigned i = 1; i < argc; i++) {
		const char *const arg = argv[i];
		if (nexttype == ARG_UNKNWN) {
			thistype = get_arg_type(arg);
		} else if (nexttype != ARG_EOAS) {
			thistype = nexttype;
			nexttype = ARG_UNKNWN;
		}

		switch(thistype) {
		case ARG_HELP:
			print_help();
			exit(0);
			break;
		case ARG_PID_VALUE:
			options.pid = arg_pid(arg);
			break;
		case ARG_FILE_VALUE:
			options.file = arg_file(arg);
			break;
		case ARG_PID:
			nexttype = ARG_PID_VALUE;
			break;
		case ARG_FILE:
			nexttype = ARG_FILE_VALUE;
			break;
		case ARG_EOAS:
			if (nexttype != ARG_EOAS) {
				nexttype = ARG_EOAS;
				break;
			}
			nexttype = ARG_EOAS;
		case ARG_VALUE:
			options.bytes = arg_value(arg, options.bytes);
			break;
		case ARG_VERBOSE:
			options.verbose = 1;
			break;
		case ARG_INVALID:
		case ARG_UNKNWN:
		default:
			LOGFATAL("invalid arg: ", arg);
			break;
		}
	}

	if (nexttype != ARG_UNKNWN && nexttype != ARG_EOAS) {
		char* msg;
		switch(nexttype) {
		case ARG_PID_VALUE:
			msg = "pid";
			break;
		case ARG_FILE_VALUE:
			msg = "file";
			break;
		default:
			msg = malloc(8 * sizeof(const char));
			sprintf(msg, "arg type %i", (int)nexttype);
			break;
		}
		LOGFATAL("missing value for: ", msg);
	}

	if (options.file == NULL && options.pid == 0) {
		LOGFATAL("no target given", "");
	}

	if (options.file != NULL && options.pid != 0) {
		LOGFATAL("only one target can be given", "");
	}

	if (options.bytes.len == 0U) {
		LOGFATAL("no search pattern given", "");
	}

	return options;
}

static unsigned arg_pid(const char *const strpid)
{
	errno = 0;
	unsigned long pid = strtoul(strpid, NULL, 0);
	if (pid == 0 || pid > UINT_MAX || errno) {
		LOGFATAL("cannot parse pid: ", strpid);
	}
	return pid;
}

static char* arg_file(const char *const file)
{
	unsigned len = strlen(file);
	if (len == 0) {
		return NULL;
	}
	char* path = malloc(len * sizeof(const char) + 1);
	return strcpy(path, file);
}

static struct bytearr arg_value(const char *const numstr, struct bytearr bytes)
{
	const struct bytearr value = convert_hexstr_to_bytes(numstr);
	return append_bytes(bytes, value);
}

static enum arg_type get_arg_type(const char arg[])
{
	enum loop_state mode = LOOP_FIRST;
	unsigned arglen = strlen(arg);

	for (unsigned i = 0; i < arglen; i++) {
		switch(mode) {
		case LOOP_FIRST:
			switch(arg[i]) {
			case '-':
				mode = LOOP_OPT;
				break;
			default:
				mode = LOOP_READ;
				break;
			}
			break;
		case LOOP_OPT:
			switch(arg[i]) {
			case '-':
				mode = LOOP_LONG_OPT;
				break;
			default:
				mode = LOOP_SHORT_OPT;
				i--;
				break;
			}
			break;
		case LOOP_LONG_OPT:
			switch(arg[i]) {
			case 'F':
			case 'f':
				mode = LOOP_OPT_LONG_F;
				break;
			case 'H':
			case 'h':
				mode = LOOP_OPT_LONG_H;
				break;
			case 'P':
			case 'p':
				mode = LOOP_OPT_LONG_P;
				break;
			case 'V':
			case 'v':
				mode = LOOP_OPT_LONG_V;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_F:
			switch(arg[i]) {
			case 'I':
			case 'i':
				mode = LOOP_OPT_LONG_FI;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_FI:
			switch(arg[i]) {
			case 'L':
			case 'l':
				mode = LOOP_OPT_LONG_FIL;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_FIL:
			switch(arg[i]) {
			case 'E':
			case 'e':
				mode = LOOP_OPT_LONG_FILE;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_H:
			switch(arg[i]) {
			case 'E':
			case 'e':
				mode = LOOP_OPT_LONG_HE;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_HE:
			switch(arg[i]) {
			case 'L':
			case 'l':
				mode = LOOP_OPT_LONG_HEL;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_HEL:
			switch(arg[i]) {
			case 'P':
			case 'p':
				mode = LOOP_OPT_LONG_HELP;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_P:
			switch(arg[i]) {
			case 'I':
			case 'i':
				mode = LOOP_OPT_LONG_PI;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_PI:
			switch(arg[i]) {
			case 'D':
			case 'd':
				mode = LOOP_OPT_LONG_PID;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_V:
			switch(arg[i]) {
			case 'E':
			case 'e':
				mode = LOOP_OPT_LONG_VE;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_VE:
			switch(arg[i]) {
			case 'R':
			case 'r':
				mode = LOOP_OPT_LONG_VER;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_VER:
			switch(arg[i]) {
			case 'B':
			case 'b':
				mode = LOOP_OPT_LONG_VERB;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_VERB:
			switch(arg[i]) {
			case 'O':
			case 'o':
				mode = LOOP_OPT_LONG_VERBO;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_VERBO:
			switch(arg[i]) {
			case 'S':
			case 's':
				mode = LOOP_OPT_LONG_VERBOS;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_LONG_VERBOS:
			switch(arg[i]) {
			case 'E':
			case 'e':
				mode = LOOP_OPT_LONG_VERBOSE;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_SHORT_OPT:
			switch(arg[i]) {
			case 'F':
			case 'f':
				mode = LOOP_OPT_SHORT_F;
				break;
			case 'H':
			case 'h':
				mode = LOOP_OPT_SHORT_H;
				break;
			case 'P':
			case 'p':
				mode = LOOP_OPT_SHORT_P;
				break;
			case 'V':
			case 'v':
				mode = LOOP_OPT_SHORT_V;
				break;
			default:
				mode = LOOP_INVALID_OPT;
				break;
			}
			break;
		case LOOP_OPT_SHORT_F:
		case LOOP_OPT_SHORT_H:
		case LOOP_OPT_SHORT_P:
		case LOOP_OPT_SHORT_V:
		case LOOP_OPT_LONG_FILE:
		case LOOP_OPT_LONG_HELP:
		case LOOP_OPT_LONG_PID:
		case LOOP_OPT_LONG_VERBOSE:
			mode = LOOP_INVALID_OPT;
			i = arglen; // To exit for-loop immediately
			break;
		case LOOP_READ:
			i = arglen; // To exit for-loop immediately
			break;
		case LOOP_INVALID_OPT:
		default:
			mode = LOOP_INVALID_OPT;
			i = arglen; // To exit for-loop immediately
			break;
		}
	}

	enum arg_type type;

	switch(mode) {
	case LOOP_LONG_OPT:
		type = ARG_EOAS;
		break;
	case LOOP_OPT_LONG_FILE:
	case LOOP_OPT_SHORT_F:
		type = ARG_FILE;
		break;
	case LOOP_OPT_LONG_HELP:
	case LOOP_OPT_SHORT_H:
		type = ARG_HELP;
		break;
	case LOOP_OPT_LONG_PID:
	case LOOP_OPT_SHORT_P:
		type = ARG_PID;
		break;
	case LOOP_OPT_LONG_VERBOSE:
	case LOOP_OPT_SHORT_V:
		type = ARG_VERBOSE;
		break;
	case LOOP_OPT:
	case LOOP_READ:
		type = ARG_VALUE;
		break;
	default:
		type = ARG_INVALID;
		break;
	}
	return type;
}
