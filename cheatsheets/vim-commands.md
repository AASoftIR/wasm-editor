# 🎯 Vim Commands Reference (for mix editor)

## Mode Switching

| Key      | From   | To          | Description          |
| -------- | ------ | ----------- | -------------------- |
| `Esc`    | Any    | Normal      | Exit to normal mode  |
| `i`      | Normal | Insert      | Insert before cursor |
| `a`      | Normal | Insert      | Insert after cursor  |
| `I`      | Normal | Insert      | Insert at line start |
| `A`      | Normal | Insert      | Insert at line end   |
| `o`      | Normal | Insert      | New line below       |
| `O`      | Normal | Insert      | New line above       |
| `v`      | Normal | Visual      | Visual selection     |
| `V`      | Normal | Visual Line | Line selection       |
| `Ctrl+E` | Any    | Toggle      | Toggle Edit mode     |

## Movement (Normal Mode)

### Basic

| Key       | Action     |
| --------- | ---------- |
| `h` / `←` | Move left  |
| `j` / `↓` | Move down  |
| `k` / `↑` | Move up    |
| `l` / `→` | Move right |

### Word Movement

| Key | Action                           |
| --- | -------------------------------- |
| `w` | Next word start                  |
| `b` | Previous word start              |
| `e` | Word end                         |
| `W` | Next WORD (whitespace-separated) |
| `B` | Previous WORD                    |
| `E` | WORD end                         |

### Line Movement

| Key | Action                      |
| --- | --------------------------- |
| `0` | Line start                  |
| `^` | First non-blank             |
| `$` | Line end                    |
| `_` | First non-blank (same as ^) |

### Document Movement

| Key     | Action       |
| ------- | ------------ |
| `gg`    | First line   |
| `G`     | Last line    |
| `{n}G`  | Go to line n |
| `{n}gg` | Go to line n |

## Operators

| Key         | Action                   |
| ----------- | ------------------------ |
| `d{motion}` | Delete                   |
| `c{motion}` | Change (delete + insert) |
| `y{motion}` | Yank (copy)              |

### Common Combinations

| Keys | Action             |
| ---- | ------------------ |
| `dw` | Delete word        |
| `d$` | Delete to line end |
| `dd` | Delete line        |
| `D`  | Delete to line end |
| `cw` | Change word        |
| `cc` | Change line        |
| `C`  | Change to line end |
| `yy` | Yank line          |
| `Y`  | Yank to line end   |

## Editing

| Key      | Action                  |
| -------- | ----------------------- |
| `x`      | Delete character        |
| `X`      | Delete character before |
| `r{c}`   | Replace with {c}        |
| `R`      | Replace mode            |
| `J`      | Join lines              |
| `u`      | Undo                    |
| `Ctrl+r` | Redo                    |
| `.`      | Repeat last change      |

## Search

| Key          | Action                   |
| ------------ | ------------------------ |
| `/{pattern}` | Search forward           |
| `?{pattern}` | Search backward          |
| `n`          | Next match               |
| `N`          | Previous match           |
| `*`          | Search word under cursor |
| `#`          | Search word backward     |

## Visual Mode

| Key      | Action            |
| -------- | ----------------- |
| `v`      | Character-wise    |
| `V`      | Line-wise         |
| `Ctrl+v` | Block-wise        |
| `o`      | Move to other end |
| `d`      | Delete selection  |
| `y`      | Yank selection    |
| `c`      | Change selection  |

## Text Objects (with d, c, y)

| Key  | Object                 |
| ---- | ---------------------- |
| `iw` | Inner word             |
| `aw` | A word (with space)    |
| `i"` | Inner quotes           |
| `a"` | A quotes (with quotes) |
| `i(` | Inner parentheses      |
| `a(` | A parentheses          |
| `i{` | Inner braces           |
| `it` | Inner tag              |

### Examples

| Keys  | Action                    |
| ----- | ------------------------- |
| `diw` | Delete inner word         |
| `ci"` | Change inner quotes       |
| `da(` | Delete around parentheses |
| `yit` | Yank inner tag            |

## Not Implemented in mix (by design)

- Macros (`q`)
- Registers (`"`)
- Marks (`m`)
- Plugins
- Vimscript
- Complex motions (`%`, `f`, `t`)

---

## VS Code Shortcuts (Edit Mode)

| Shortcut       | Action                        |
| -------------- | ----------------------------- |
| `Ctrl+P`       | Quick open file               |
| `Ctrl+Shift+P` | Command palette               |
| `Ctrl+D`       | Select word / next occurrence |
| `Ctrl+Shift+L` | Select all occurrences        |
| `Alt+Click`    | Add cursor                    |
| `Ctrl+/`       | Toggle comment                |
| `Ctrl+F`       | Find                          |
| `Ctrl+H`       | Find and replace              |
| `Ctrl+G`       | Go to line                    |
| `Ctrl+Z`       | Undo                          |
| `Ctrl+Shift+Z` | Redo                          |
| `Ctrl+S`       | Save                          |
