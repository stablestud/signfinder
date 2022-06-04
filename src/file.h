#ifndef FILE_H
#define FILE_H

#include "args.h"

struct hits search_file(const struct args options);
void file_print_result(struct hits matches, struct args options);

#endif /* FILE_H */
