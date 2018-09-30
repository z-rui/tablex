#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dict.h"

static int
tnode_compar(const void *key, const struct tst_node *n)
{
	unsigned k1, k2;
	const struct tnode *tn;

	k1 = *(const int *) key;
	tn = container_of(n, struct tnode, tst);
	k2 = tn->key;
	return (k1 < k2) ? -1 : (k1 == k2) ? 0 : 1;
}

static void
clist_link_node(struct clist *l, struct cnode *prev, struct cnode *next)
{
	if (prev)
		prev->next = next;
	else
		l->head = next;
	if (next)
		next->prev = prev;
	else
		l->tail = prev;
}

void
dict_rotate(struct dict *d, struct tnode_leaf *tn)
{
	struct clist *l;
	struct cnode *cn;

	l = &tn->list;
	if (l->head == l->tail)
		return;
	cn = l->head;
	l->head = l->head->next;
	clist_link_node(&d->list, l->head->prev, l->head);
	clist_link_node(&d->list, cn, l->tail->next);
	clist_link_node(&d->list, l->tail, cn);
	l->tail = cn;
}

void
dict_add_leaf(struct dict *d, struct cnode *cn, struct tnode_leaf *tn)
{
	struct cnode *prev_cn, *next_cn;

	if (tn->list.head) {
		prev_cn = tn->list.tail;
		next_cn = prev_cn->next;
		tn->list.tail = cn;
	} else {
		struct tst_node *prev;

		tn->list.head = tn->list.tail = cn;
		prev = tst_prev(&tn->tst);
		if (!prev) {
			prev_cn = 0;
			next_cn = d->list.head;
		} else {
			tn = container_of(prev, struct tnode_leaf, tst);
			prev_cn = tn->list.tail;
			next_cn = prev_cn->next;
		}
	}
	/* these assertions are very useful to find bugs */
	if (next_cn)
		assert(strcmp(cn->kseq, next_cn->kseq) <= 0);
	if (prev_cn)
		assert(strcmp(prev_cn->kseq, cn->kseq) <= 0);
	clist_link_node(&d->list, cn, next_cn);
	clist_link_node(&d->list, prev_cn, cn);
}

struct tnode_leaf *
dict_mkpath(struct dict *d, const char *key)
{
	struct tst_node *parent, **link;
	struct tnode *tn;
	unsigned k;

	link = &d->root;
	parent = 0;
	do {
		k = (unsigned char) *key;
		if (*link == 0) {
			struct tnode *tn;

			tn = calloc(1, k ? sizeof (struct tnode) : sizeof (struct tnode_leaf));
			tn->key = k;
			tst_link_node(&tn->tst, parent, link);
		}
		tn = container_of(*link, struct tnode, tst);
		if (tn->key == k)
			key++;
		parent = &tn->tst;
		link = tst_step(parent, &k, tnode_compar);
	} while (k != 0 || tn->key != 0);

	//tst_splay(&tn->tst, &d->root);
	return (struct tnode_leaf *) tn;
}

struct tnode *tnode_find_step(struct tnode *cur, unsigned key)
{
	while (cur) {
		struct tst_node *n;
		unsigned k;

		n = *tst_step(&cur->tst, &key, tnode_compar);
		k = cur->key;
		cur = n ? container_of(n, struct tnode, tst) : 0;
		if (k == key)
			break;
	}
	return cur;
}

static void tnode_free(struct tst_node *n)
{
	if (!n) return;
	tnode_free(n->l);
	tnode_free(n->m);
	tnode_free(n->r);
	free(container_of(n, struct tnode, tst));
}

void dict_load_line(struct dict *d, const char *line, size_t len)
{
	char *buf;
	char *kseq, *cseq;
	struct tnode_leaf *tn;
	struct cnode *cn;

	buf = malloc(len + 1);
	memcpy(buf, line, len);
	buf[len] = '\0';

	kseq = strtok(buf, " ");
	cseq = strtok(0, " ");
	if (!cseq) { /* invalid line */
		free(buf);
		return;
	}
	tn = dict_mkpath(d, kseq);
	do {
		cn = malloc(sizeof *cn);
		cn->kseq = kseq;
		cn->cseq = cseq;
		dict_add_leaf(d, cn, tn);
	} while ((cseq = strtok(0, " ")));
}

void dict_finalize(struct dict *d)
{
	struct cnode *cn, *next;

	tnode_free(d->root);
	for (cn = d->list.head; cn; cn = next) {
		next = cn->next;
		if (!next || next->kseq != cn->kseq)
			free(cn->kseq);
		free(cn);
	}
}

#ifndef NDEBUG
static void tnode_dump(const struct tnode *n, int level)
{
	if (!n) return;

	fprintf(stderr, "\n%*s(tnode %p: ",
		level, "", n);
	if (n->key) {
		fprintf(stderr, "key = %u (%c), ",
			n->key, n->key);
	} else {
		const struct tnode_leaf *tl;

		tl = (const struct tnode_leaf *) n;
		fprintf(stderr, "list = (%p, %p), ",
			tl->list.head, tl->list.tail);
	}
	fputs(", l = ", stderr);
	tnode_dump(container_of(n->tst.l, struct tnode, tst), level + 1);
	fputs(", m = ", stderr);
	tnode_dump(container_of(n->tst.m, struct tnode, tst), level + 1);
	fputs(", r = ", stderr);
	tnode_dump(container_of(n->tst.r, struct tnode, tst), level + 1);
	fprintf(stderr, "\n%*s)", level, "");
}

static void cnode_dump(const struct cnode *n)
{
	fprintf(stderr, "cnode %p: %s %s\n", n, n->kseq, n->cseq);
}

void dict_dump(const struct dict *d)
{
	const struct cnode *cn;

	tnode_dump(container_of(d->root, struct tnode, tst), 0);
	fputc('\n', stderr);
	for (cn = d->list.head; cn; cn = cn->next)
		cnode_dump(cn);
}
#endif
