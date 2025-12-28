# 🔧 Lesson 04: C → WASM with Emscripten

## 🎯 Goal

Learn the **real-world** workflow: writing C and compiling to WASM using **Emscripten**.

This is how production WASM apps are built!

## 🤔 Why C for WASM?

| Feature              | Why It Matters                   |
| -------------------- | -------------------------------- |
| No garbage collector | Predictable performance          |
| Small binaries       | Fast loading                     |
| Emscripten           | Battle-tested C → WASM toolchain |
| Memory control       | Direct memory management         |
| Massive ecosystem    | Reuse existing C libraries       |
| Industry standard    | Used by Figma, AutoCAD, Unity    |

## 🛠️ Setup (One-time)

### Install Emscripten

**Windows:**

```bash
# Clone the SDK
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# Install and activate
emsdk install latest
emsdk activate latest

# Add to PATH (run in every new terminal, or add to profile)
emsdk_env.bat
```

**macOS:**

```bash
brew install emscripten
```

**Linux:**

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

### Verify Installation

```bash
emcc --version
# emcc (Emscripten gcc/clang-like replacement) 3.x.x
```

## 📁 Project Structure

```
04-c-wasm/
├── src/
│   └── main.c          # C source code
├── www/
│   ├── index.html      # Web demo
│   └── main.js         # Generated (after build)
│   └── main.wasm       # Generated (after build)
├── build.sh            # Build script
└── README.md
```

## 📝 Step-by-Step

### Step 1: Write C Code

```c
// src/main.c
#include <emscripten.h>
#include <string.h>
#include <stdlib.h>

// EMSCRIPTEN_KEEPALIVE prevents dead code elimination
// It marks functions to be exported to JavaScript

EMSCRIPTEN_KEEPALIVE
int add(int a, int b) {
    return a + b;
}

EMSCRIPTEN_KEEPALIVE
int multiply(int a, int b) {
    return a * b;
}

// Fibonacci - the classic benchmark
EMSCRIPTEN_KEEPALIVE
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// Working with arrays
EMSCRIPTEN_KEEPALIVE
int sum_array(int* arr, int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

// String processing
EMSCRIPTEN_KEEPALIVE
char* greet(const char* name) {
    static char buffer[256];
    snprintf(buffer, sizeof(buffer), "Hello, %s! 👋", name);
    return buffer;
}

// Memory allocation for JS interop
EMSCRIPTEN_KEEPALIVE
int* create_array(int size) {
    return (int*)malloc(size * sizeof(int));
}

EMSCRIPTEN_KEEPALIVE
void free_array(int* arr) {
    free(arr);
}
```

### Step 2: Build with Emscripten

```bash
emcc src/main.c -o www/main.js \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS="['_add', '_multiply', '_fibonacci', '_sum_array', '_greet', '_create_array', '_free_array', '_malloc', '_free']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'UTF8ToString', 'stringToUTF8', 'getValue', 'setValue']" \
    -s ALLOW_MEMORY_GROWTH=1 \
    -O2
```

### Build Flags Explained

| Flag                          | Description                                          |
| ----------------------------- | ---------------------------------------------------- |
| `-s WASM=1`                   | Output WebAssembly (not just asm.js)                 |
| `-s EXPORTED_FUNCTIONS`       | Functions to expose to JS (prefix with `_`)          |
| `-s EXPORTED_RUNTIME_METHODS` | Helper functions for interop                         |
| `-s ALLOW_MEMORY_GROWTH=1`    | Dynamic memory allocation                            |
| `-O2`                         | Optimization level (0-3, s for size, z for min size) |

### Step 3: Use in JavaScript

```html
<!DOCTYPE html>
<html>
	<head>
		<title>C → WASM Demo</title>
	</head>
	<body>
		<h1>C → WASM Demo</h1>
		<div id="output"></div>

		<script>
			// Module object is created by Emscripten
			var Module = {
				onRuntimeInitialized: function () {
					// Direct function calls (fastest)
					console.log("add(5, 3) =", Module._add(5, 3));
					console.log("fibonacci(10) =", Module._fibonacci(10));

					// Using cwrap for cleaner API
					const multiply = Module.cwrap("multiply", "number", [
						"number",
						"number",
					]);
					console.log("multiply(4, 5) =", multiply(4, 5));

					// String handling
					const greet = Module.cwrap("greet", "string", ["string"]);
					console.log(greet("World"));

					document.getElementById(
						"output"
					).innerHTML = `<p>add(5, 3) = ${Module._add(5, 3)}</p>
                     <p>fibonacci(10) = ${Module._fibonacci(10)}</p>
                     <p>${greet("WASM")}</p>`;
				},
			};
		</script>
		<script src="main.js"></script>
	</body>
</html>
```

## 🔥 Performance Comparison

Try the demo to see C/WASM vs JavaScript performance!

```
fibonacci(40):
  JavaScript: ~1200ms
  C/WASM:     ~400ms  (3x faster!)
```

## 🧠 Memory Management

### Passing Arrays from JS to WASM

```javascript
// Allocate memory in WASM heap
const arr = [1, 2, 3, 4, 5];
const ptr = Module._malloc(arr.length * 4); // 4 bytes per int

// Copy data to WASM memory
for (let i = 0; i < arr.length; i++) {
	Module.setValue(ptr + i * 4, arr[i], "i32");
}

// Call C function
const sum = Module._sum_array(ptr, arr.length);
console.log("Sum:", sum); // 15

// Free memory!
Module._free(ptr);
```

### Using TypedArrays (More Efficient)

```javascript
// Direct memory view
const heap = new Int32Array(Module.HEAP32.buffer);
const ptr = Module._malloc(5 * 4);
const offset = ptr / 4; // Convert byte offset to i32 offset

// Write directly
heap[offset] = 1;
heap[offset + 1] = 2;
heap[offset + 2] = 3;
heap[offset + 3] = 4;
heap[offset + 4] = 5;

const sum = Module._sum_array(ptr, 5);
Module._free(ptr);
```

## 📊 Optimization Levels

| Level    | Flag  | Description                    |
| -------- | ----- | ------------------------------ |
| Debug    | `-O0` | No optimization, fast build    |
| Basic    | `-O1` | Basic optimizations            |
| Release  | `-O2` | Recommended for production     |
| Maximum  | `-O3` | Aggressive (may increase size) |
| Size     | `-Os` | Optimize for size              |
| Min Size | `-Oz` | Minimum size (slower)          |

## 🧪 Advanced: Calling JavaScript from C

```c
#include <emscripten.h>

// Direct JavaScript call
EMSCRIPTEN_KEEPALIVE
void log_to_console() {
    EM_ASM({
        console.log('Hello from C via EM_ASM!');
    });
}

// JavaScript call with parameters
EMSCRIPTEN_KEEPALIVE
int get_random_int(int max) {
    return EM_ASM_INT({
        return Math.floor(Math.random() * $0);
    }, max);
}

// Call JavaScript function and get result
EMSCRIPTEN_KEEPALIVE
double call_js_math() {
    return EM_ASM_DOUBLE({
        return Math.PI * Math.E;
    });
}
```

## 📁 Files in This Lesson

```
04-c-wasm/
├── README.md           ← You are here
├── src/
│   └── main.c          ← C source code
├── www/
│   └── index.html      ← Demo page
├── build.sh            ← Build script (Linux/macOS)
└── build.bat           ← Build script (Windows)
```

## 🎮 Try It!

```bash
# Build
./build.sh    # or build.bat on Windows

# Serve (WASM requires a server)
python -m http.server 8080 --directory www

# Open http://localhost:8080
```

## ✅ What You Learned

1. **Emscripten** is the standard C → WASM compiler
2. **EMSCRIPTEN_KEEPALIVE** exports functions to JavaScript
3. **Module.cwrap** creates clean JavaScript wrappers
4. **Memory management** requires manual malloc/free
5. **EM_ASM** lets you call JavaScript from C

## ➡️ Next Lesson

[05 - Memory & Performance Deep Dive →](../05-memory-performance/README.md)

---

💡 **Pro Tip:** For complex projects, use a Makefile or CMake with Emscripten's `emcmake` for better build management.
