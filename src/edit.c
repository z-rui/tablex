#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edit.h"

void edit_load(struct edit *e, const char *path)
{
	FILE *f;
	char buf[4096];

	f = fopen(path, "r");
	if (!f)
		return;
	while (fgets(buf, sizeof buf, f)) {
		size_t len;

		len = strlen(buf);
		if (len > 0 && buf[len-1] == '\n')
			len--;
		dict_load_line(&e->d, buf, len);
	}
	fclose(f);
}

static void setcur(struct edit *e, struct tnode *cur)
{
	e->cur = cur;
	if (cur) {
		struct tst_node *min, *max;
		struct tnode_leaf *min_tn, *max_tn;

		min = tst_min(&cur->tst);
		min_tn = container_of(min, struct tnode_leaf, tst);
		max = tst_max(&cur->tst);
		max_tn = container_of(max, struct tnode_leaf, tst);

		e->candidates.head = min_tn->list.head;
		e->candidates.tail = max_tn->list.tail;
		e->ccur = e->candidates.head;
		tst_splay(&cur->tst, &e->d.root);
	} else {
		e->candidates.head = e->candidates.tail = 0;
		e->ccur = 0;
	}
}

void edit_reset(struct edit *e)
{
	e->buflen = 0;
	setcur(e, container_of(e->d.root, struct tnode, tst));
}

int edit_empty(const struct edit *e)
{
	return e->buflen == 0;
}

int edit_ismatch(struct edit *e)
{
	return (e->ccur && e->buflen == strlen(e->ccur->kseq));
}

int edit_isonly(struct edit *e)
{
	return (e->ccur && e->candidates.head == e->candidates.tail);
}

int edit_addkey(struct edit *e, unsigned key)
{
	struct tnode *tn;

	if (e->buflen == EDIT_BUFSIZ) /* no space */
		return 0;
	tn = tnode_find_step(e->cur, key);
	if (tn) {
		e->buf[e->buflen++] = key;
		setcur(e, tn);
		return 1;
	} else if (edit_ismatch(e) &&
		(tn = tnode_find_step(container_of(e->d.root,
			struct tnode, tst), key))) {
		edit_select(e, 0);
		e->buf[0] = key;
		e->buflen = 1;
		setcur(e, tn);
		return 1;
	}
	return 0;
}

void edit_backspace(struct edit *e)
{
	if (e->buflen > 0) {
		struct tst_node *n;

		n = tst_back(&e->cur->tst);
		e->buflen--;
		setcur(e, container_of(n, struct tnode, tst));
	}
}

int edit_shift(struct edit *e, int n)
{
	struct cnode *cand;

	cand = e->ccur;
	if (!cand)
		return 0;
	if (n > 0) {
		while (n--) {
			if (cand == e->candidates.tail)
				return 0;
			cand = cand->next;
		}
	} else if (n < 0) {
		while (n++) {
			if (cand == e->candidates.head)
				return 0;
			cand = cand->prev;
		}
	}
	e->ccur = cand;
	return 1;
}

void edit_select(struct edit *e, unsigned i)
{
	struct cnode *cand;

	cand = e->ccur;
	if (!cand)
		return;
	while (i--) {
		if (cand == e->candidates.tail)
			return; /* invalid */
		cand = cand->next;
	}
	edit_commit(e->data, cand->cseq);

	if (cand->kseq[0] == '\'' || cand->kseq[0] == '"')
		dict_rotate(&e->d, (struct tnode_leaf *) e->cur);
	edit_reset(e);
}

void edit_show(struct edit *e)
{
	struct cnode *cand;

	e->buf[e->buflen] = '\0';
	edit_buffer(e->data, (const char *) e->buf, e->buflen);
	cand = e->ccur;
	while (edit_candidate(e->data, cand->cseq, cand->kseq + e->buflen)
			&& cand != e->candidates.tail) {
		cand = cand->next;
	}
}

struct edit *edit_new(void *data)
{
	struct edit *e;

	e = calloc(1, sizeof (struct edit));
	e->data = data;
	return e;
}

void edit_free(struct edit *e)
{
	dict_finalize(&e->d);
	free(e);
}
