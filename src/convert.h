#ifndef CONVERT_H
#define CONVERT_H

#include "args.h"

struct bytearr convert_hexstr_to_bytes(const char *const str);
struct bytearr append_bytes(struct bytearr src1, struct bytearr src2);
void* alloc_copy_mem(const void *const src, const unsigned size);
char* alloc_copy_string(const char* src);
void* realloc_append_mem(void *const dest, const unsigned long destlen, const void *const src, const unsigned long srclen);

#endif /* CONVERT_H */
