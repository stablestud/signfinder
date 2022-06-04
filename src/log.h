#include "log.def"

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#include "args.h"
#include "main.h"


void log_print(const char msg[]);
void log_println(const char msg[]);
void log_error(const char msg[], const char suffix[]);
void log_fatal(const char msg[], const char suffix[]);

void print_help(void);
void print_bytes(const struct bytearr bytes);

char* strappend(const char str1[], const char str2[]);

#endif /* LOG_H */
