# 🔗 WASM Resources

## Official Documentation

- [WebAssembly.org](https://webassembly.org/) - Official site
- [MDN WebAssembly Guide](https://developer.mozilla.org/en-US/docs/WebAssembly) - Comprehensive docs
- [WASM Spec](https://webassembly.github.io/spec/) - Technical specification

## Learning Resources

### Tutorials

- [Emscripten Documentation](https://emscripten.org/docs/getting_started/index.html) - Official Emscripten guide
- [WebAssembly by Example](https://wasmbyexample.dev/) - Interactive examples
- [MDN WASM C Guide](https://developer.mozilla.org/en-US/docs/WebAssembly/C_to_wasm) - C to WASM tutorial

### Videos

- [Lin Clark's WASM Cartoon Intro](https://hacks.mozilla.org/2017/02/a-cartoon-intro-to-webassembly/)
- [Surma's Raw WASM](https://www.youtube.com/watch?v=u0Jgz6QVJqg) - Building without frameworks

### Articles

- [Understanding WASM Text Format](https://developer.mozilla.org/en-US/docs/WebAssembly/Understanding_the_text_format)
- [Memory in WASM](https://hacks.mozilla.org/2017/07/memory-in-webassembly-and-why-its-safer-than-you-think/)

## Tools

### Build Tools

| Tool                                              | Purpose                 | Install                                                                         |
| ------------------------------------------------- | ----------------------- | ------------------------------------------------------------------------------- |
| [Emscripten](https://emscripten.org/)             | C/C++ → WASM            | See [install guide](https://emscripten.org/docs/getting_started/downloads.html) |
| [WABT](https://github.com/WebAssembly/wabt)       | WAT ↔ WASM              | `npm install -g wabt`                                                           |
| [AssemblyScript](https://www.assemblyscript.org/) | TypeScript-like → WASM  | `npm install assemblyscript`                                                    |
| [Clang](https://clang.llvm.org/)                  | C/C++ → WASM (via LLVM) | Part of LLVM                                                                    |

### Optimization

| Tool                                                                     | Purpose           | Install                |
| ------------------------------------------------------------------------ | ----------------- | ---------------------- |
| [wasm-opt](https://github.com/WebAssembly/binaryen)                      | Optimize WASM     | Part of Binaryen       |
| [wasm-strip](https://github.com/nicolo-ribaudo/nicolo-ribaudo.github.io) | Remove debug info | Part of WABT           |
| [Twiggy](https://github.com/nicolo-ribaudo/nicolo-ribaudo.github.io)     | Size profiler     | `cargo install twiggy` |

### Debugging

| Tool                                                                   | Purpose                 |
| ---------------------------------------------------------------------- | ----------------------- |
| Chrome DevTools                                                        | Built-in WASM debugging |
| Firefox DevTools                                                       | Built-in WASM debugging |
| [wasm2wat](https://github.com/nicolo-ribaudo/nicolo-ribaudo.github.io) | Disassemble WASM        |

## Libraries

### C/Emscripten

| Library                                                                                           | Purpose                  |
| ------------------------------------------------------------------------------------------------- | ------------------------ |
| [Emscripten Ports](https://emscripten.org/docs/compiling/Building-Projects.html#emscripten-ports) | Pre-compiled C libraries |
| [SDL2](https://wiki.libsdl.org/SDL2/EmscriptenCompilation)                                        | Graphics/Input for games |
| [OpenGL ES](https://emscripten.org/docs/porting/multimedia_and_graphics/OpenGL-support.html)      | 3D graphics              |

### JavaScript

| Library                                                               | Purpose                  |
| --------------------------------------------------------------------- | ------------------------ |
| [comlink](https://github.com/nicolo-ribaudo/nicolo-ribaudo.github.io) | Web Worker communication |
| [pako](https://github.com/nicolo-ribaudo/nicolo-ribaudo.github.io)    | WASM compression         |

## Real-World Examples

### Open Source Projects Using WASM

| Project                                                                                                 | Uses WASM For     |
| ------------------------------------------------------------------------------------------------------- | ----------------- |
| [Figma](https://www.figma.com/)                                                                         | Canvas rendering  |
| [AutoCAD Web](https://web.autocad.com/)                                                                 | CAD engine        |
| [Google Earth](https://earth.google.com/web/)                                                           | 3D globe          |
| [Photoshop Web](https://www.adobe.com/products/photoshop.html)                                          | Image processing  |
| [eBay Barcode Scanner](https://tech.ebayinc.com/engineering/webassembly-at-ebay-a-real-world-use-case/) | Image processing  |
| [Squoosh](https://squoosh.app/)                                                                         | Image compression |

### Example Repositories

- [nicolo-ribaudo/nicolo-ribaudo.github.io](nicolo-ribaudo/nicolo-ribaudo.github.io) - Cargo web template
- [nicolo-ribaudo/nicolo-ribaudo.github.io](nicolo-ribaudo/nicolo-ribaudo.github.io) - Game of Life tutorial
- [nicolo-ribaudo/nicolo-ribaudo.github.io](nicolo-ribaudo/nicolo-ribaudo.github.io) - Various WASM examples

## Online Tools

| Tool                                               | Purpose                |
| -------------------------------------------------- | ---------------------- |
| [WasmFiddle](https://nicolo-ribaudo.github.io/)    | Online WASM playground |
| [WebAssembly Studio](https://webassembly.studio/)  | Online IDE             |
| [WAT2WASM Demo](https://nicolo-ribaudo.github.io/) | Convert WAT online     |
| [WASM Explorer](https://nicolo-ribaudo.github.io/) | Explore compiled WASM  |

## Community

- [r/WebAssembly](https://www.reddit.com/r/WebAssembly/) - Reddit community
- [Emscripten Discord](https://discord.gg/emscripten) - Emscripten community
- [awesome-wasm](https://github.com/mbasso/awesome-wasm) - Awesome WASM list

## Future: WASI

WebAssembly System Interface - Run WASM outside browsers!

- [WASI.dev](https://wasi.dev/) - Official WASI site
- [Wasmtime](https://wasmtime.dev/) - WASI runtime
- [Wasmer](https://wasmer.io/) - Universal WASM runtime
