# tablex

Yet another table-based input method.

# How to compile

1. Clone the repository to somewhere.
2. `make curses`.
3. `./curses` to test how the input method works.

# How to compile for IBus.

1. Grep `@PKGDATADIR@`.
2. Modify the files accordingly, and save without `.in` suffix.
3. `make ibus`.

# How to install for IBus locally

1. Make a symlink in `~/.local/share/ibus/component/` to `tablex.xml`.
2. Run
```
IBUS_COMPONENT_PATH=~/.local/share/ibus/component:/usr/share/ibus/component \
exec ibus write-cache
```
3. Restart your IBus.

# How to use my own table

1. Put your table in `mb/`.
2. Look at `py.txt`, change `mb/py.txt` to what you've just put in 'mb/', and save as `name_of_your_table.txt`.
3. Modify `table.xml` and create a new `<engine>` block with
`<name>tablex:name_of_your_table</name>`.
4. You may need to write IBus cache if you installed this locally.
5. Restart your IBus.

# How to switch Chinese/English mode, half/full width mode, etc.

- Shift key is used for switching Chinese/English mode.  This is hardcoded.
- You can also use IBus key combinations (`<Super> Space` by default) to switch among input methods.
- Switching between 🌙/🌕 or `,.`/`，。` is not implemented,  but you can modify `mb/punct.txt` to change the default behavior.

# Why another input method?

Because I ran into several issues with the existing input methods.

- I don't like "smart" input methods.  However, input methods try to be smart these days.
- `ibus-table` has a different punctuation mapping that is incompatible with `ibus-libpinyin`.
I don't like that mapping but unfortunately it's hardcoded and thus not customizable.
