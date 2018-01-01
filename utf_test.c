#include <stdio.h>

#include "utf.h"

int main()
{
	const char *p = "木直中绳，𫐓以为轮，其曲中规。";
	unsigned cp;
	int n;

	puts(p);
	printf("len = %u\n", (unsigned) utf8_len(p));
	while (*p && (n = utf8_decode(p, &cp)) > 0) {
		fwrite(p, 1, n, stdout);
		printf("\tU+%04X\tlen=%d\n", cp, n);
		p += n;
	}
	return 0;
}
