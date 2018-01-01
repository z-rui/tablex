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

extern struct edit *edit_new(void *data);
extern void edit_free(struct edit *);

extern void edit_load(struct edit *e, const char *path);
extern void edit_reset(struct edit *);
extern int edit_empty(const struct edit *);
extern int edit_addkey(struct edit *, unsigned);
extern void edit_backspace(struct edit *);
extern int edit_shift(struct edit *, int);
extern void edit_select(struct edit *, unsigned);
extern int edit_ismatch(struct edit *);
extern int edit_isonly(struct edit *);
extern void edit_show(struct edit *e);

/* interfaces (implemented elsewhere) */

extern void edit_commit(void *data, const char *);
extern void edit_buffer(void *data, const char *, int len);
extern int edit_candidate(void *data, const char *, const char *);

#endif /* EDIT_H */
