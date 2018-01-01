#ifndef TST_H
#define TST_H

#include <stddef.h>

#define container_of(x,t,f) ((void *)((char *)x-offsetof(t,f)))

struct tst_node {
	struct tst_node *p, *l, *m, *r;
};

extern struct tst_node **tst_step(struct tst_node *, const void *,
		int (*)(const void *, const struct tst_node *));
extern struct tst_node *tst_back(struct tst_node *);
extern struct tst_node *tst_min(struct tst_node *);
extern struct tst_node *tst_max(struct tst_node *);

extern void tst_link_node(struct tst_node *, struct tst_node *,
		struct tst_node **);
extern struct tst_node * tst_prev(struct tst_node *n);
extern void tst_splay(struct tst_node *, struct tst_node **);

#endif /* TST_H */
