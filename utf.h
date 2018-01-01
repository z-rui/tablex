#ifndef UTF_H
#define UTF_H

#include <stddef.h>

extern int utf8_decode(const char *, unsigned *);
extern size_t utf8_len(const char *);

#endif /* UTF_H */
