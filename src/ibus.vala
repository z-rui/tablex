using Config;

namespace TableX {
	class Engine : IBus.Engine {
		const uint kCandidateCount = 10;
		const uint kHintColor = 0xff9515b5U;
		const int kOrientation = IBus.Orientation.SYSTEM;

		IBus.LookupTable lookup_table;
		IBus.PropList plist;
		IBus.Property prop_state;
		Edit editor;

		bool is_chinese {
			get { return prop_state.state == IBus.PropState.CHECKED; }
			set {
				if (value) {
					prop_state.state = IBus.PropState.CHECKED;
					prop_state.label = new IBus.Text
						.from_static_string("Chinese Mode");
					prop_state.symbol = new IBus.Text
						.from_unichar(0x4e2d); // 中
				} else {
					prop_state.state = IBus.PropState.UNCHECKED;
					prop_state.label = new IBus.Text
						.from_static_string("English Mode");
					prop_state.symbol = new IBus.Text
						.from_static_string("En");
				}
				update_property(prop_state);
			}
		}
		uint lastkey;

		construct {
			lookup_table = new IBus.LookupTable(
				kCandidateCount,
				/*cursor_pos=*/0,
				/*cursor_visible=*/false,
				/*round=*/false);
			lookup_table.set_orientation(kOrientation);
			prepare_editor();
			prepare_properties();
			is_chinese = true;
		}
		void prepare_editor()
		{
			editor = new Edit(this);
			var name = get_name();
			var idx = name.last_index_of_char(':');
			if (idx != -1)
				name = name.substring(idx+1);
			var f = FileStream.open(@"$PACKAGE_DATADIR/$name.txt", "r");
			if (f == null) {
				stderr.printf("%s not found\n", name);
			}
			string line;
			while ((line = f.read_line()) != null) {
				editor.load(@"$PACKAGE_DATADIR/$line");
			}
			editor.reset();
		}
		void prepare_properties()
		{
			plist = new IBus.PropList();
			prop_state = new IBus.Property(
				"InputMode", IBus.PropType.NORMAL,
				new IBus.Text.from_static_string(""), null,
				new IBus.Text.from_static_string("Click to switch Chinese/English mode"),
				true, true,
				IBus.PropState.CHECKED, null);
			plist.append(prop_state);
		}
		void key_accepted()
		{
			if (editor.isonly() && editor.ismatch())
				editor.select(0);
			refresh_ui();
		}
		void key_rejected(uint keyval)
		{
			switch (keyval) {
				case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8':
				case '9': case '0':
					keyval = (keyval == '0') ? 9 : keyval - '1';
					if (keyval >= kCandidateCount)
						return;
					editor.select(keyval - '1');
					break;
				case IBus.Escape:
					editor.reset();
					break;
				case IBus.BackSpace:
					editor.backspace();
					break;
				case '=':
				case IBus.Page_Down:
					page_down();
					break;
				case '-':
				case IBus.Page_Up:
					page_up();
					break;
				case ' ':
					editor.select(0);
					break;
				default:
					/* Unknown key.
					 * We do nothing, not even refreshing the UI. */
					return;
			}
			refresh_ui();
		}
		public override void page_down()
		{
			editor.shift((int) lookup_table.get_page_size());
			refresh_ui();
		}
		public override void page_up()
		{
			editor.shift(-(int) lookup_table.get_page_size());
			refresh_ui();
		}
		public override void focus_out()
		{
			editor.reset();
			refresh_ui();
			base.focus_out();
		}
		public override void focus_in()
		{
			register_properties(plist);
			base.focus_in();
		}
		public override void set_content_type(uint purpose, uint hints)
		{
			switch (purpose) {
				case IBus.InputPurpose.FREE_FORM:
				case IBus.InputPurpose.NAME:
					break;
				case IBus.InputPurpose.ALPHA:
				case IBus.InputPurpose.DIGITS:
				case IBus.InputPurpose.NUMBER:
				case IBus.InputPurpose.PHONE:
				case IBus.InputPurpose.URL:
				case IBus.InputPurpose.EMAIL:
				case IBus.InputPurpose.PASSWORD:
				case IBus.InputPurpose.PIN:
					is_chinese = false;
					break;
			}
			base.set_content_type(purpose, hints);
		}
		public override void candidate_clicked(uint index, uint button, uint state)
		{
			if (button == 1 && state == 0) {
				editor.select(index);
				refresh_ui();
			}
		}
		void toggle_chinese()
		{
				var is_chinese = this.is_chinese;
				if (is_chinese)
					focus_out(); // or commit ?
				this.is_chinese = !is_chinese;
		}
		public override void property_activate(string name, uint state)
		{
			switch (name) {
				case "state":
					toggle_chinese();
					break;
			}
		}
		public override bool process_key_event(uint keyval, uint keycode, uint state)
		{
			state &= ~IBus.ModifierType.MOD2_MASK;  // ignore numlock
			state &= ~IBus.ModifierType.SHIFT_MASK;
			if ((state & IBus.ModifierType.RELEASE_MASK) != 0 &&
				(keyval == IBus.Shift_L || keyval == IBus.Shift_R)
				&& keyval == lastkey) {
				toggle_chinese();
			}
			lastkey = keyval;
			if (state != 0)
				return false;
			var unicode = IBus.keyval_to_unicode(keyval);
			if (unicode != 0 && is_chinese && editor.addkey(unicode)) {
				key_accepted();
			} else if (editor.empty()) {
				return false; // pass through
			} else {
				key_rejected(keyval);
			}
			return true;
		}
		public int candidate(string text, string hint)
		{
			var start = text.char_count();
			var end = start + hint.char_count();

			IBus.Text ibustext = new IBus.Text.from_string(text + hint);
			IBus.Attribute attr = IBus.attr_foreground_new(
				kHintColor, start, end);
			ibustext.append_attribute(attr.get_attr_type(),
				attr.get_value(),
				attr.get_start_index(),
				(int) attr.get_end_index());

			lookup_table.append_candidate(ibustext);
			
			return (int) (lookup_table.get_number_of_candidates() < lookup_table.get_page_size());
		}

		void refresh_ui()
		{
			lookup_table.clear();
			if (editor.empty()) {
				hide_lookup_table();
				hide_auxiliary_text();
				hide_preedit_text();
			} else {
				editor.show();
				// (weird bug)
				// the candidate windows some times does not follow the cursor
				// if the call to update_lookup_table is not duplicated
				update_lookup_table(lookup_table, true);
				update_lookup_table(lookup_table, true);
			}
		}
	}
}

static bool exec_by_ibus;

const OptionEntry[] options = {
	{"ibus", 'i', 0, OptionArg.NONE, ref exec_by_ibus,
		"Component is executed by ibus", null},
	{null}
};

public int main(string[] args)
{
	var context = new OptionContext ("- ibus tablex");
	context.add_main_entries (options, "ibus-tablex");
	try {
		context.parse (ref args);
	} catch (OptionError e) {
		stderr.printf ("%s\n", e.message);
		return 1;
	}

	var ibus = new IBus.Bus();
	if (!ibus.is_connected()) {
		stderr.printf("Cannot connect to ibus-daemon!\n");
		return 1;
	}
	ibus.disconnected.connect(() => { IBus.quit(); });

	var com = new IBus.Component.from_file(@"$PACKAGE_DATADIR/tablex.xml");
	var factory = new IBus.Factory(ibus.get_connection());
	foreach(var eng in com.get_engines())
		factory.add_engine(eng.name, typeof (TableX.Engine));

	if (!exec_by_ibus)
		ibus.register_component(com);
	else
		ibus.request_name("org.freedesktop.IBus.TableX", 0);

	IBus.main();
	return 0;
}

public void edit_commit(void *data, string s)
{
	unowned TableX.Engine engine = (TableX.Engine) data;
	engine.commit_text(new IBus.Text.from_string(s));
}

public void edit_buffer(void *data, string s, int len)
{
	unowned TableX.Engine engine = (TableX.Engine) data;
	var text = new IBus.Text.from_string(s);

	engine.update_preedit_text(text, text.get_length(), true);
	//engine.update_auxiliary_text(text, true);
}

public int edit_candidate(void *data, string s, string hint)
{
	unowned TableX.Engine engine = (TableX.Engine) data;
	return engine.candidate(s, hint);
}

// vim: ts=4:sw=4
