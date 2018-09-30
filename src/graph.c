/* Graphing utility
 *
 * Usage:
 *
 *     graph <dict file> > graph.d
 *
 * Then type one line of key sequence (to alter the splay tree).
 * The metapost code for the tree will be written to graph.d.
 *
 * Then run metapost to get the figure:
 *
 *     mpost graph
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edit.h"

static int node_id = 0;

static int
dump_tree_graph(struct tst_node *n)
{
	int id, l, m, r;

	if (n->l)
		l = dump_tree_graph(n->l);
	if (n->m)
		m = dump_tree_graph(n->m);
	id = ++node_id;
	printf("K%d=%d;\n", id, ((struct tnode *) n)->key);
	if (n->r)
		r = dump_tree_graph(n->r);
	if (n->l)
		printf("P%d=%d;\n", l, id);
	if (n->m)
		printf("P%d=%d;\nM%d=%d;\n", m, id, id, m);
	if (n->r)
		printf("P%d=%d;\n", r, id);
	return id;
}

int
main(int argc, char *argv[])
{
	struct edit *ed;
	char buf[128];
	char *p;

	if (argc < 2)
		return 1;

	ed = edit_new(0);
	edit_load(ed, argv[1]);
	if (!fgets(buf, sizeof buf, stdin))
		return 1;
	edit_reset(ed);
	for (p = buf; *p != '\0' && *p != '\n'; p++)
		edit_addkey(ed, *p);
	dump_tree_graph(ed->d.root);
	printf("N=%d;\n", node_id);
	edit_free(ed);

	return 0;
}

void edit_commit(void *data, const char *s)
{
}
