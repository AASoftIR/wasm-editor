# 🔮 mix

> A minimal, ultra-fast, dual-mode code editor powered by WebAssembly

```
███╗   ███╗██╗██╗  ██╗
████╗ ████║██║╚██╗██╔╝
██╔████╔██║██║ ╚███╔╝
██║╚██╔╝██║██║ ██╔██╗
██║ ╚═╝ ██║██║██╔╝ ╚██╗
╚═╝     ╚═╝╚═╝╚═╝   ╚═╝
```

**mix** = Vim + VS Code, built for focus.

## 🎯 Philosophy

- **Minimalism over features**
- **Speed over extensibility**
- **Intent over configuration**
- **Focus over customization**

## ⚡ Goals

| Metric            | Target           |
| ----------------- | ---------------- |
| Startup time      | < 100ms          |
| Input latency     | < 16ms           |
| Large file (10MB) | Smooth scrolling |
| Memory usage      | < 50MB baseline  |

## 🎮 Two Modes

### Command Mode (Vim-inspired)

- Modal editing (Normal/Insert/Visual)
- Core Vim motions: `w`, `b`, `e`, `$`, `^`, `gg`, `G`
- Operators: `d`, `c`, `y`
- Search: `/`, `n`, `N`
- No macros, no registers, no Vimscript

### Edit Mode (VS Code-inspired)

- Multi-cursor editing
- `Ctrl+P` → File navigation
- `Ctrl+Shift+P` → Command palette
- Standard shortcuts (copy, paste, find)
- No extension marketplace

### Mode Switching

- `Esc` → Command Mode
- `Ctrl+E` → Edit Mode

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      UI Layer (TS/JS)                       │
│   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       │
│   │ Render  │  │  Input  │  │  Theme  │  │ Commands│       │
│   │ Engine  │  │ Handler │  │ Manager │  │ Palette │       │
│   └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘       │
│        │            │            │            │             │
│        └────────────┴────────────┴────────────┘             │
│                          │                                  │
│                          ▼                                  │
├─────────────────── WASM Bridge ────────────────────────────┤
│                          │                                  │
│                          ▼                                  │
│                    WASM Core (C)                            │
│   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       │
│   │  Text   │  │  Vim    │  │  Syntax │  │   LSP   │       │
│   │ Buffer  │  │ Commands│  │  Parser │  │ Handler │       │
│   │(PTable) │  │         │  │(TreeSit)│  │         │       │
│   └─────────┘  └─────────┘  └─────────┘  └─────────┘       │
└─────────────────────────────────────────────────────────────┘
```

## 📁 Project Structure

```
mix/
├── core/                   # C WASM core
│   ├── build.sh            # Build script (Unix)
│   ├── build.bat           # Build script (Windows)
│   └── src/
│       ├── main.c          # Entry point & Vim commands
│       ├── buffer.c        # Piece table text buffer
│       ├── buffer.h
│       ├── cursor.c        # Cursor management
│       ├── cursor.h
│       ├── undo.c          # Undo/redo history
│       ├── undo.h
│       ├── search.c        # Search & replace
│       └── search.h
│
├── ui/                     # TypeScript UI
│   ├── package.json
│   └── src/
│       ├── main.ts
│       ├── editor/
│       │   ├── Editor.ts
│       │   ├── Cursor.ts
│       │   └── Selection.ts
│       ├── modes/
│       │   ├── CommandMode.ts
│       │   └── EditMode.ts
│       ├── commands/
│       │   └── Palette.ts
│       └── themes/
│           ├── vs-dark.css
│           └── vim-zen.css
│
├── public/
│   └── index.html
│
└── README.md
```

## 🛠️ Tech Stack

| Component   | Technology                         |
| ----------- | ---------------------------------- |
| Core engine | C + Emscripten                     |
| Text buffer | Piece table (custom C)             |
| Syntax      | Tree-sitter (WASM)                 |
| UI          | Vanilla TypeScript                 |
| Styling     | CSS Variables                      |
| Storage     | IndexedDB + File System Access API |

## 🚀 Getting Started

### Prerequisites

- Emscripten SDK
- Node.js 18+
- Modern browser

### Development

```bash
# Build WASM core
cd core
./build.sh    # or build.bat on Windows

# Install UI dependencies
cd ../ui
npm install

# Start dev server
npm run dev
```

### Build for Production

```bash
npm run build
```

## 📋 MVP Features

### Phase 1: Core Engine ✨

- [x] Text buffer with piece table
- [x] Cursor and selection management
- [x] Basic insert/delete operations
- [ ] Undo/redo history

### Phase 2: Modes

- [x] Vim command parser
- [x] Core motions (h, j, k, l, w, b, e, $, ^)
- [ ] Operators (d, c, y)
- [ ] VS Code shortcuts

### Phase 3: Intelligence

- [ ] Tree-sitter integration
- [ ] Syntax highlighting
- [ ] LSP client

### Phase 4: Polish

- [ ] Two themes
- [ ] Focus mode
- [ ] Startup optimization

## 🎨 Themes

### VS Dark Minimal

```css
--bg: #1e1e1e;
--fg: #d4d4d4;
--keyword: #569cd6;
--string: #ce9178;
--comment: #6a9955;
```

### Vim Zen

```css
--bg: #0a0a0a;
--fg: #888888;
--keyword: #888888;
--string: #888888;
--error: #ff6b6b;
--type: #d4d4d4;
```

## 📖 Design Decisions

### Why Piece Table for Text Buffer?

- O(1) insert/delete operations
- Memory efficient (original text never copied)
- Perfect for undo/redo (just track piece changes)
- Used by VS Code itself!

### Why C for WASM Core?

- Maximum control over memory
- Smallest possible binary size
- No runtime overhead (no GC)
- Emscripten is battle-tested
- Used by Figma, AutoCAD, Unity

### Why Tree-sitter?

- Incremental parsing
- Error tolerant
- Compiles to WASM

### Why No Extensions?

- Faster startup
- Predictable behavior
- No dependency hell

## 🤝 Contributing

This is a learning project! Feel free to:

1. Fork and experiment
2. Open issues for questions
3. Submit PRs with improvements

## 📄 License

MIT

---

_Built with ❤️ and WebAssembly_
