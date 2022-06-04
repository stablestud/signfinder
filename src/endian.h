#ifndef ENDIAN_H
#define ENDIAN_H

#define ENDIAN_BE	1
#define ENDIAN_LE	0
#define ENDIAN_UNKNWN	-1

/* Return the endianness of the current system */
int check_endianness(void);

#endif /* ENDIAN_H */
