# 🚀 WebAssembly Learning Hub

> From Zero to WASM Hero — A comprehensive learning repository for web developers

```
██╗    ██╗ █████╗ ███████╗███╗   ███╗
██║    ██║██╔══██╗██╔════╝████╗ ████║
██║ █╗ ██║███████║███████╗██╔████╔██║
██║███╗██║██╔══██║╚════██║██║╚██╔╝██║
╚███╔███╔╝██║  ██║███████║██║ ╚═╝ ██║
 ╚══╝╚══╝ ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝
```

## 📚 What's Inside

| Directory         | Description                               |
| ----------------- | ----------------------------------------- |
| `📁 learn/`       | Step-by-step WASM tutorials (start here!) |
| `📁 examples/`    | Practical code examples                   |
| `📁 mix/`         | Real project: Dual-mode WASM code editor  |
| `📁 tools/`       | CLI setup tool & utilities                |
| `📁 cheatsheets/` | Quick reference guides                    |

## 🎯 Learning Path

```
┌─────────────────────────────────────────────────────────────┐
│  START HERE                                                 │
│  ↓                                                          │
│  01. What is WASM? ──────────────────────────────────────┐  │
│  02. Your First WASM (hand-written WAT) ──────────────┤  │
│  03. WASM + JavaScript Bridge ───────────────────────┤  │
│  04. C → WASM (Emscripten power!) ──────────────────┤  │
│  05. Memory & Performance ───────────────────────────────┤  │
│  06. Real Project: mix editor ───────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## ⚡ Quick Start

```bash
# Clone this repo
git clone <this-repo>
cd web

# Use our CLI tool to set up everything
python tools/wasm-cli.py setup

# Or manually navigate to learn/
cd learn/01-what-is-wasm
```

## 🛠️ Prerequisites

- **Required:** Modern browser (Chrome/Firefox/Edge)
- **Required:** Basic HTML/CSS/JavaScript knowledge
- **Optional:** Python 3.8+ (for CLI tool)
- **Optional:** Emscripten (for C → WASM compilation)

## 🎮 The Mix Project

This repo includes a real-world project: **mix** — a minimal, dual-mode WASM code editor.

- 🎯 Vim + VS Code mental models
- ⚡ Loads in <100ms
- 🔧 C → WASM core (Emscripten)
- 🌐 Fully offline-capable

See [mix/README.md](mix/README.md) for project details.

## 📖 Learning Philosophy

1. **No Assembly Required** — You don't need to know Assembly to use WASM
2. **Hands-On First** — Every concept has runnable examples
3. **Web Developer Friendly** — Explained from a JS/web perspective
4. **Progressive Complexity** — Start simple, build up gradually

## 🗺️ Roadmap

- [x] Basic WASM concepts
- [x] JavaScript interop examples
- [x] C to WASM pipeline (Emscripten)
- [ ] Advanced memory management
- [ ] Threading with Web Workers
- [ ] WASI (WebAssembly System Interface)

---

**Made with ❤️ for web developers who want to go fast**
