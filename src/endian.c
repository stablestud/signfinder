#include <stdint.h>

#include "endian.h"

int check_endianness(void)
{
	const uint32_t dword = (0x88 << 24) | (0x44 << 16) | (0x22 << 8) | (0x11);
	const uint8_t *const bytes = (const uint8_t *const)&dword;

	if ((bytes[0] & 0x88) && (bytes[1] & 0x44) && (bytes[2] & 0x22) && (bytes[3] & 0x11)) {
		return ENDIAN_BE;
	} else if ((bytes[0] & 0x11) && (bytes[1] & 0x22) && (bytes[2] & 0x44) && (bytes[3] & 0x88)) {
		return ENDIAN_LE;
	} else {
		return ENDIAN_UNKNWN;
	}
}
