#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "convert.h"
#include "endian.h"
#include "log.h"

static uintptr_t count_hbyte_len(const char *const str);
uint8_t* convert_int_to_bytes(unsigned long num, unsigned len);

struct bytearr convert_hexstr_to_bytes(const char *const str)
{
	errno = 0;
	struct bytearr bytes;
	unsigned long num = strtoul(str, 0, 16);
	if (errno) {
		LOGFATAL("cannot parse value: ", str);
	}

	uintptr_t len = count_hbyte_len(str);
	bytes.len = len;
	bytes.data = convert_int_to_bytes(num, len);

	return bytes;
}

uint8_t* convert_int_to_bytes(unsigned long num, unsigned len)
{
	uint8_t* buffer = malloc(len);
	int endian = check_endianness();
	switch(endian) {
	case ENDIAN_BE:
		memcpy(buffer, (uint8_t *)&num + (sizeof(num) - len), len);
		break;
	case ENDIAN_LE:
		memcpy(buffer, (uint8_t *)&num, len);
		break;
	case ENDIAN_UNKNWN:
	default:
		LOGFATAL("unsupported system endianness", "");
		break;
	}
	return buffer;
}

struct bytearr append_bytes(struct bytearr src1, struct bytearr src2)
{
	struct bytearr bytes = { src1.len + src2.len, calloc(src1.len + src2.len, sizeof(uint8_t)) };
	memcpy(bytes.data, src1.data, src1.len);
	memcpy(bytes.data + src1.len, src2.data, src2.len);
	if (src1.data != NULL)
		free(src1.data);
	if (src2.data != NULL)
		free(src2.data);
	return bytes;
}

static uintptr_t count_hbyte_len(const char *const str)
{
	unsigned slen = strlen(str);
	uintptr_t len = 0U;
	for (unsigned i = 0; i < slen; i++) {
		switch(str[i]) {
		case ' ':
		case '\n':
		case '\t':
			if (len == 0U) {
				continue;
			}
			i = slen;	// force exit loop
			break;
		case '0':
			if (len == 0U) {
				continue;
			}
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'a':
		case 'A':
		case 'b':
		case 'B':
		case 'c':
		case 'C':
		case 'd':
		case 'D':
		case 'e':
		case 'E':
		case 'f':
		case 'F':
			len++;
			break;
		case 'x':
		case 'X':
			if (len == 0U) {
				continue;
			}
		default:
			LOGFATAL("cannot parse value: ", str);
			break;
		}
	}

	if (len == 0U) {
		len = 1U;
	} else {
		len = len / 2U + len % 2U;
	}
	return len;
}

char* alloc_copy_string(const char *const src)
{
	unsigned len = strlen(src);
	char* string;
	if (len == 0) {
		return calloc(1, 1);
	}

	string = malloc(len * sizeof(const char) + 1 * sizeof(const char));
	if (string == NULL) {
		LOGFATAL("cannot allocate mem for string: ", src);
	}
	return strcpy(string, src);
}

void* alloc_copy_mem(const void *const src, const unsigned size)
{
	void* newmem = malloc(size);
	return memcpy(newmem, src, size);
}

void* realloc_append_mem(void *const dest, const unsigned long destlen, const void *const src, const unsigned long srclen)
{
	if (dest == NULL || src == NULL) {
		LOGFATAL("cannot allocate nullptr", "");
	}
	void *const ptr = realloc((void *)dest, destlen + srclen);
	if (ptr == NULL) {
		LOGFATAL("failed to allocate memory", "");
	}
	memcpy((void *)((uintptr_t)ptr + destlen), src, srclen);
	return ptr;
}
