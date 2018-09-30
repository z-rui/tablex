#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

#include "imapi.h"
#include "keycode.h"

#include "config.h"
#include "edit.h"
#include "utf.h"

static unsigned cursorx, cursory;
static Info fbinfo;
struct edit *ed;
const char *preedit, *candidate;
unsigned preedit_w, candidate_w;
static int converting = 0;

static void im_active()
{
}

static void im_deactive()
{
	Rectangle rect = { 0, 0, 0, 0 };
	set_im_window(0, rect);
	converting = 0;
	edit_reset(ed);
}

static unsigned u8swidth(const char *s)
{
	unsigned w = 0;
	unsigned ch;
	unsigned n;

	while (n = utf8_decode(s, &ch), ch != '\0') {
		if (n) {
			w += wcwidth(ch);
			s += n;
		} else { /* invalid utf-8 */
			w++;
			s++;
		}
	}
	return w;
}

void edit_buffer(void *data, const char *s, int len)
{
	preedit = s;
	preedit_w = u8swidth(s);
}

int edit_candidate(void *data, const char *s, const char *hint)
{
	candidate = s;
	candidate_w = u8swidth(s);
	return 0;
}

void edit_commit(void *data, const char *s)
{
	put_im_text(candidate, strlen(candidate));
	cursorx += candidate_w * fbinfo.fontWidth;
}

static void draw_im()
{
	Rectangle rect;
	const char *s;
	unsigned w;

	if (edit_empty(ed)) {
		s = "";
		w = 0;
	} else if (converting) {
		s = candidate;
		w = candidate_w;
	} else {
		s = preedit;
		w = preedit_w;
	}
	rect.x = cursorx;
	rect.y = cursory - fbinfo.fontHeight;
	rect.w = w * fbinfo.fontWidth;
	rect.h = fbinfo.fontHeight;

	set_im_window(0, rect);
	draw_text(rect.x, rect.y,
		converting ? Red : Yellow, Black, s, strlen(s));
}

static void process_key(char *keys, unsigned len)
{
	int ch, i;

	for (i = 0; i < len; i++) {
		ch = keys[i];
		if (converting) {
			switch (ch) {
			case ' ':
				if (edit_shift(ed, 1))
					if (!edit_ismatch(ed))
						edit_shift(ed, -1);
				break;
			default:
				i--;
				/* fallthrough */
			case '\r':
				edit_select(ed, 0);
				/* fall through */
			case '\177':
				converting = 0;
				while (edit_shift(ed, -1)) ;
				break;
			}
		} else if (edit_addkey(ed, ch)) {
			if (edit_isonly(ed) && edit_ismatch(ed))
				converting = 1;
		} else if (edit_empty(ed)) {
			put_im_text(keys, len);
		} else {
			switch (ch) {
			case ' ':
				converting = 1;
				break;
			case '\r':
				edit_reset(ed);
				break;
			case '\177':
				edit_backspace(ed);
				break;
			}
		}
	}
	edit_show(ed);
	draw_im();
}

static void im_show(unsigned winid)
{
	draw_im();
}

static void im_hide()
{
}

static void cursor_pos_changed(unsigned x, unsigned y)
{
	cursorx = x;
	cursory = y;
}

static void update_fbterm_info(Info *info)
{
	fbinfo = *info;
}

static ImCallbacks cbs = {
	im_active, // .active
	im_deactive, // .deactive
	im_show,	 // .show_ui
	im_hide, // .hide_ui
	process_key, // .send_key
	cursor_pos_changed, // .cursor_position
	update_fbterm_info, // .fbterm_info
	update_term_mode // .term_mode
};

static void loadmb(const char *filename)
{
	FILE *f;
	char buf[1024] = PACKAGE_DATADIR"/";
	size_t len0, len;

	f = fopen(filename, "r");
	if (!f) return;
	len0 = strlen(buf);
	while(fgets(buf + len0, sizeof buf - len0, f)) {
		len = strlen(buf);
		if (len > 0 && buf[len-1] == '\n')
			buf[len-1] = '\0';
		edit_load(ed, buf);
	}
	fclose(f);
}

int main()
{
	setlocale(LC_ALL, "");

	ed = edit_new(NULL);
	loadmb(PACKAGE_DATADIR"/fbterm.txt");
	edit_reset(ed);

	register_im_callbacks(cbs);
	connect_fbterm(0);
	while (check_im_message()) ;

	edit_free(ed);
	return 0;
}
