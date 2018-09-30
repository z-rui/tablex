#ifndef EDIT_H
#define EDIT_H

#define EDIT_BUFSIZ 255

#include "dict.h"

struct edit {
	struct dict d;
	struct tnode *cur;
	struct clist candidates;
	struct cnode *ccur;
	void *data;
	unsigned char buf[EDIT_BUFSIZ+1];
	int buflen;
};

/* methods */

extern struct edit *edit_new(void *);
extern void edit_free(struct edit *);

extern void edit_load(struct edit *, const char *);
extern void edit_reset(struct edit *);
extern int edit_empty(const struct edit *);
extern int edit_addkey(struct edit *, unsigned);
extern void edit_backspace(struct edit *);
extern int edit_shift(struct edit *, int);
extern void edit_select(struct edit *, unsigned);
extern int edit_ismatch(struct edit *);
extern int edit_isonly(struct edit *);
extern void edit_show(struct edit *);

/* interfaces (implemented elsewhere) */

extern void edit_commit(void *, const char *);
extern void edit_buffer(void *, const char *, int);
extern int edit_candidate(void *, const char *, const char *);

#endif /* EDIT_H */
