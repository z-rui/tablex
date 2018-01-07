#ifndef DICT_H
#define DICT_H

#include "tst.h"

struct clist {
	struct cnode *head, *tail;
};

struct dict {
	struct tst_node *root;
	struct clist list;
};

struct tnode {
	struct tst_node tst;
	unsigned key;
};

struct tnode_leaf {
	struct tst_node tst;
	unsigned key;
	struct clist list;
};

struct cnode {
	struct tnode *parent;
	struct cnode *prev, *next;
	char *kseq, *cseq;
};

extern struct tnode *tnode_find_step(struct tnode *, unsigned);

extern void dict_rotate(struct dict *, struct tnode_leaf *);
extern void dict_load_line(struct dict *, const char *, size_t);
extern void dict_finalize(struct dict *);

/* for debugging use */

extern void dict_dump(const struct dict *);

#endif /* DICT_H */
