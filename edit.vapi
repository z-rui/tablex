[CCode (cheader_filename = "edit.h")]
namespace TableX {
	[Compact]
	[CCode (cname = "struct edit", cprefix = "edit_")]
	class Edit {
		[CCode (cname = "edit_new")]
		public Edit(void *data);
		public void load(string path);
		public void reset();
		public bool empty();
		public bool addkey(uint key);
		public void backspace();
		public bool shift(int n);
		public void select(uint i);
		public bool ismatch();
		public bool isonly();
		public void show();
	}
}
