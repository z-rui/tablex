#include "tst.h"

struct tst_node **
tst_step(struct tst_node *n, const void *key,
		int (*compar)(const void *, const struct tst_node *))
{
	int cmp;

	cmp = compar(key, n);
	return (cmp < 0) ? &n->l : (cmp == 0) ? &n->m : &n->r;
}

struct tst_node *
tst_back(struct tst_node *n)
{
	do {
		n = n->p;
	} while (n->p && n != n->p->m);
	return n;
}

struct tst_node *
tst_min(struct tst_node *n)
{
	for (;;) {
		if (n->l)
			n = n->l;
		else if (n->m)
			n = n->m;
		else break;
	}
	return n;
}

struct tst_node *
tst_max(struct tst_node *n)
{
	for (;;) {
		if (n->r)
			n = n->r;
		else if (n->m)
			n = n->m;
		else break;
	}
	return n;
}

void
tst_link_node(struct tst_node *n, struct tst_node *parent,
		struct tst_node **link)
{
	if (n)
		n->p = parent;
	*link = n;
}

static struct tst_node *rightmost(struct tst_node *n)
{
	for (;;) {
		if (n->r)
			n = n->r;
		else if (n->m)
			n = n->m;
		else
			break;
	}
	return n;
}

struct tst_node *
tst_prev(struct tst_node *n)
{
	if (n->l)
		return rightmost(n->l);
	for (;;) {
		struct tst_node *p;

		if (!(p = n->p))
			return 0;
		if (p->r == n)
			return (p->m) ? rightmost(p->m) : p;
		else if (p->m == n && p->l)
			return rightmost(p->l);
		n = p;
	}
}

void
tst_splay(struct tst_node *n, struct tst_node **root)
{
	struct tst_node *p, *gp, *ggp, **link;

	while ((p = n->p)) {
		if (n == p->m) {
			n = p;
			continue;
		}
		gp = p->p;
		if (gp && p != gp->m) {
			if ((ggp = gp->p) == 0)
				link = root;
			else if (gp == ggp->l)
				link = &ggp->l;
			else if (gp == ggp->r)
				link = &ggp->r;
			else
				link = &ggp->m;
			tst_link_node(n, ggp, link);
		} else {
			if (!gp)
				link = root;
			else
				link = &gp->m;
			tst_link_node(n, gp, link);
		}
		if (n == p->l) {
			if (gp) {
				if (p == gp->l) {
					tst_link_node(p->r, gp, &gp->l);
					tst_link_node(gp, p, &p->r);
				} else if (p == gp->r) {
					tst_link_node(n->l, gp, &gp->r);
					tst_link_node(gp, n, &n->l);
				}
			}
			tst_link_node(n->r, p, &p->l);
			tst_link_node(p, n, &n->r);
		} else {
			if (gp) {
				if (p == gp->r) {
					tst_link_node(p->l, gp, &gp->r);
					tst_link_node(gp, p, &p->l);
				} else if (p == gp->l) {
					tst_link_node(n->r, gp, &gp->l);
					tst_link_node(gp, n, &n->r);
				}
			}
			tst_link_node(n->l, p, &p->r);
			tst_link_node(p, n, &n->l);
		}
	}
}
