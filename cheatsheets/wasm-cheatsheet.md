# 📋 WebAssembly Cheatsheet

## Quick Reference

### WAT (WebAssembly Text Format)

#### Module Structure

```wat
(module
  ;; Imports
  (import "env" "log" (func $log (param i32)))
  (import "js" "mem" (memory 1))

  ;; Function definition
  (func $name (param $x i32) (param $y i32) (result i32)
    local.get $x
    local.get $y
    i32.add
  )

  ;; Export
  (export "name" (func $name))
)
```

#### Data Types

| Type  | Description    | Size    |
| ----- | -------------- | ------- |
| `i32` | 32-bit integer | 4 bytes |
| `i64` | 64-bit integer | 8 bytes |
| `f32` | 32-bit float   | 4 bytes |
| `f64` | 64-bit float   | 8 bytes |

#### Numeric Operations

```wat
;; Integer operations
i32.add      ;; +
i32.sub      ;; -
i32.mul      ;; *
i32.div_s    ;; / (signed)
i32.div_u    ;; / (unsigned)
i32.rem_s    ;; % (signed)
i32.rem_u    ;; % (unsigned)

;; Comparison
i32.eq       ;; ==
i32.ne       ;; !=
i32.lt_s     ;; < (signed)
i32.lt_u     ;; < (unsigned)
i32.gt_s     ;; > (signed)
i32.gt_u     ;; > (unsigned)
i32.le_s     ;; <= (signed)
i32.ge_s     ;; >= (signed)

;; Bitwise
i32.and      ;; &
i32.or       ;; |
i32.xor      ;; ^
i32.shl      ;; <<
i32.shr_s    ;; >> (signed)
i32.shr_u    ;; >> (unsigned)
```

#### Control Flow

```wat
;; If-else
(if (result i32)
  (i32.gt_s (local.get $x) (i32.const 0))
  (then (i32.const 1))
  (else (i32.const 0))
)

;; Loop
(block $break
  (loop $continue
    ;; ... code ...
    br_if $break      ;; break if condition
    br $continue      ;; continue loop
  )
)
```

#### Memory Operations

```wat
;; Load from memory
i32.load           ;; load 4 bytes
i32.load8_s        ;; load 1 byte (signed)
i32.load8_u        ;; load 1 byte (unsigned)
i32.load16_s       ;; load 2 bytes (signed)

;; Store to memory
i32.store          ;; store 4 bytes
i32.store8         ;; store 1 byte
i32.store16        ;; store 2 bytes
```

---

### JavaScript API

#### Loading WASM

```javascript
// From fetch
const response = await fetch('module.wasm');
const bytes = await response.arrayBuffer();
const { instance } = await WebAssembly.instantiate(bytes, imports);

// From Uint8Array
const binary = new Uint8Array([0x00, 0x61, 0x73, 0x6d, ...]);
const { instance } = await WebAssembly.instantiate(binary, imports);

// Streaming (fastest for large files)
const { instance } = await WebAssembly.instantiateStreaming(
  fetch('module.wasm'),
  imports
);
```

#### Imports Object

```javascript
const imports = {
	env: {
		// Functions WASM can call
		log: (value) => console.log(value),
		abort: () => {
			throw new Error("abort");
		},
	},
	js: {
		// Shared memory
		mem: new WebAssembly.Memory({ initial: 1 }),
		// Shared table
		table: new WebAssembly.Table({ initial: 1, element: "anyfunc" }),
	},
};
```

#### Memory Operations

```javascript
// Create memory
const memory = new WebAssembly.Memory({
	initial: 1, // 1 page = 64KB
	maximum: 10, // max 10 pages
});

// Access as typed arrays
const bytes = new Uint8Array(memory.buffer);
const ints = new Int32Array(memory.buffer);
const floats = new Float64Array(memory.buffer);

// Grow memory
memory.grow(1); // Add 1 page

// Write string
const encoder = new TextEncoder();
const encoded = encoder.encode("hello");
bytes.set(encoded, 0); // Write at offset 0

// Read string
const decoder = new TextDecoder();
const str = decoder.decode(bytes.slice(0, 5));
```

---

### C → WASM (Emscripten)

#### Basic Export

```c
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
int add(int a, int b) {
    return a + b;
}
```

#### Strings

```c
#include <emscripten.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

EMSCRIPTEN_KEEPALIVE
char* greet(const char* name) {
    char* result = malloc(256);
    sprintf(result, "Hello, %s!", name);
    return result;
}
```

#### Arrays

```c
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
int sum(int* values, int length) {
    int total = 0;
    for (int i = 0; i < length; i++) {
        total += values[i];
    }
    return total;
}

EMSCRIPTEN_KEEPALIVE
void double_array(int* values, int length) {
    for (int i = 0; i < length; i++) {
        values[i] *= 2;
    }
}
```

#### Call JavaScript

```c
#include <emscripten.h>

// Call JS inline
void log_from_c(int value) {
    EM_ASM({
        console.log('From C:', $0);
    }, value);
}

// Or import JS functions
EM_JS(double, get_random, (), {
    return Math.random();
});
```

#### Build Commands

```bash
# Basic build
emcc src/main.c -o www/main.js -s WASM=1

# With exported functions
emcc src/main.c -o www/main.js \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS="['_add', '_greet', '_sum']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']"

# Optimized build
emcc src/main.c -o www/main.js -s WASM=1 -O3

# Size optimized
emcc src/main.c -o www/main.js -s WASM=1 -Os

# With memory settings
emcc src/main.c -o www/main.js \
    -s WASM=1 \
    -s INITIAL_MEMORY=16MB \
    -s ALLOW_MEMORY_GROWTH=1
```

---

### Common Patterns

#### Passing Strings JS → WASM (with Emscripten)

```javascript
// JavaScript side - using ccall (Emscripten helper)
const result = Module.ccall(
	"greet", // function name
	"string", // return type
	["string"], // argument types
	["World"] // arguments
);
console.log(result); // "Hello, World!"

// Or using cwrap for repeated calls
const greet = Module.cwrap("greet", "string", ["string"]);
console.log(greet("World"));
```

#### Returning Strings WASM → JS

```c
// C side - Emscripten handles this automatically
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
const char* get_message() {
    return "Hello from C!";
}
```

#### Error Handling

```c
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
int divide(int a, int b, int* error) {
    if (b == 0) {
        *error = 1; // Set error flag
        return 0;
    }
    *error = 0;
    return a / b;
}
```

---

### Performance Tips

1. **Batch operations** - One WASM call with array > many calls
2. **Minimize copies** - Use shared memory when possible
3. **Align data** - 4-byte for i32, 8-byte for f64
4. **Profile first** - Only optimize hot paths
5. **Use optimized builds** - `emcc -O3` for speed, `-Os` for size

### Size Optimization

```bash
# Optimize binary with Emscripten
emcc src/main.c -o www/main.js -Os -s WASM=1

# Use wasm-opt (from Binaryen) for further optimization
wasm-opt -Os -o output.wasm input.wasm

# Strip debug info
wasm-strip module.wasm
```

### Debugging

```javascript
// Enable debug info in build
// emcc -g src/main.c -o www/main.js

// Browser DevTools
// 1. Open DevTools → Sources
// 2. Enable WASM debugging
// 3. Set breakpoints in .wat

// Console logging
console.log(new Uint8Array(Module.HEAPU8.buffer));

// Use EM_ASM for debug output from C
// EM_ASM({ console.log('Debug:', $0); }, value);
```
