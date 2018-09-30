#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <ncurses.h>

#include "dict.h"
#include "edit.h"

struct context {
	struct edit *ed;
	unsigned char cstack[2048]; // large enough???
	int sp;
	int cand_no;
	WINDOW *ime_win;
	int col;
};

void show_candidates(struct context *ctx)
{
	ctx->cand_no = 1;
	edit_show(ctx->ed);
	ctx->cstack[ctx->sp] = ctx->cand_no - 1;
}

void edit_buffer(void *data, const char *s, int len)
{
	struct context *ctx;
	WINDOW *w;

	ctx = data;
	w = ctx->ime_win;
	waddnstr(w, (const char *) s, len);
	waddstr(w, "    ");
}

int edit_candidate(void *data, const char *s, const char *hint)
{
	struct context *ctx;
	WINDOW *w;
	int x, y;
	int oldx;

	ctx = data;
	w = ctx->ime_win;

	getyx(w, y, oldx);
	wprintw(w, "%d. %s%s ", ctx->cand_no++ % 10, s, hint);
	getyx(w, y, x);
	if (x > ctx->col - 4) {
		ctx->cand_no--;
		wmove(w, y, oldx);
		wclrtoeol(w);
		return 0;
	}
	if (ctx->cand_no > 10)
		return 0;
	return 1;
}

int main()
{
	static struct context ctx[1];
	struct edit *ed;
	int ch;
	int row, col;

	ed = ctx->ed = edit_new(ctx);
	edit_load(ed, "mb/punct.txt");
	edit_load(ed, "mb/py.txt");
	edit_reset(ed);

	setlocale(LC_ALL, "");

	initscr();			/* Start curses mode 		*/
	noecho();
	cbreak();
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

	refresh();
	getmaxyx(stdscr, row, col);
	ctx->ime_win = newwin(1,col,row-1,0);
	ctx->col = col;
#if 1
	while ((ch = getchar()) != 4) {
		if (edit_addkey(ed, ch)) {
			/* 自动上屏 */
			if (edit_isonly(ed) && edit_ismatch(ed))
				edit_select(ed, 0);
		} else if (edit_empty(ed)) {
			switch (ch) {
			case 0177:
				addch('\b');
				if (wcwidth(inch()) > 1) {
					addch('\b');
					delch();
				}
				delch();
				break;
			case 014: /* ^L */
				clear();
				refresh();
				break;
			case 025: /* ^U */
			{
				int y, x;
				getyx(stdscr, y, x);
				move(y, 0);
				clrtoeol();
				(void) x;
				break;
			}
			case '\r':
				addch('\n');
			default:
				addch(ch);
				break;
			}
			refresh();
		} else {
			switch (ch) {
			case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				edit_select(ed, ch - '1');
				break;
			case '0':
				edit_select(ed, 9);
				break;
			case 0177:
				edit_backspace(ed);
				break;
			case '=':
				if (edit_shift(ed, ctx->cstack[ctx->sp]))
					ctx->sp++;
				break;
			case '-':
				if (ctx->sp > 0)
					edit_shift(ed, -ctx->cstack[--ctx->sp]);
				break;
			case ' ':
				edit_select(ed, 0);
				break;
			case 27:
				edit_reset(ed);
				break;
			}
		}
		wclear(ctx->ime_win);
		/*mvwprintw(ctx->ime_win, 0, col-4,
			ctx->ime_enable ? "  中" : "  英");*/
		wmove(ctx->ime_win, 0, 0);
		if (!edit_empty(ed)) {
			show_candidates(ctx);
			wmove(ctx->ime_win, 0, ed->buflen);
		}
		wrefresh(ctx->ime_win);
		if (edit_empty(ed))
			refresh();
	}
#endif
	edit_free(ed);

	delwin(ctx->ime_win);
	endwin();			/* End curses mode		  */
	return 0;
}

void edit_commit(void *data, const char *s)
{
	addstr(s);
	refresh();
}
