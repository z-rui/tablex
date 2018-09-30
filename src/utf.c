#include "utf.h"

int utf8_decode(const char *s, unsigned *out)
{
	unsigned char ch;
	int len;
	int i;

	ch = *s++;
	if (!(ch & 0x80)) {
		*out = ch;
		return 1;	/* 1 byte (ASCII) */
	}
	if (!(ch & 0x40))
		return 0;	/* invalid */
	if (!(ch & 0x20))
		len = 2;
	else if (!(ch & 0x10))
		len = 3;
	else if (!(ch & 0x8))
		len = 4;
	else
		return 0;	/* invalid */
	*out = ch & (0x7f >> len);
	for (i = 2; i <= len; i++) {
		ch = *s++;
		if ((ch & 0xc0) != 0x80)
			return 0;	/* invalid */
		*out = (*out << 6) | (ch & 0x3f);
	}
	return len;
}

size_t utf8_len(const char *s)
{
	size_t len = 0, n;
	unsigned cp;

	while ((n = utf8_decode(s, &cp)) && cp) {
		s += n;
		len++;
	}
	return len;
}
