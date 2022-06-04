#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "main.h"

static void log_fd(const char prefix[], const char msg[], const char suffix[], FILE *const fd)
{
	fprintf(fd, "%s%s%s", prefix, msg, suffix);
}

void log_println(const char msg[])
{
	log_fd("", msg, "\n", stdout);
}

void log_print(const char msg[])
{
	log_fd("", msg, "", stdout);
}

void log_error(const char msg[], const char suffix[])
{
	log_fd(msg, suffix, "\n", stderr);
}

void log_fatal(const char msg[], const char suffix[])
{
	log_error(msg, suffix);
	exit(1);
}

void print_help(void)
{
	log_println(NAME " - Find specific bytes in a remote process or files");
	log_println(NAME " <options> -- <bytes..>");
	log_println("Options <options>:");
	log_println("\t--help,    -h\t\t- show this help screen");
	log_println("\t--verbose, -v\t\t- print more info");
	log_println("\t--pid,  -p <pid>\t- process to search in");
	log_println("\t--file, -f <file>\t- file to search in");
	log_println("Search bytes <bytes..> in hexadecimal");
	log_println("\ti.E.: 0xf4782 0x6723, 90 f7 e5 e6 00 e8");
}

void print_bytes(const struct bytearr bytes)
{
	char buf[4];
	log_print("Byte pattern (");
	snprintf(buf, sizeof(buf) / sizeof(buf[0]), "%llu", bytes.len);
	log_print(buf);
	log_print(" bytes): ");
	for (unsigned i = 0; i < bytes.len; i++) {
		snprintf(buf, sizeof(buf) / sizeof(buf[0]), "%.2x ", bytes.data[i]);
		log_print(buf);
	}
	log_println("");
}
