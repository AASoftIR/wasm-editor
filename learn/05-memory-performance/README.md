# 🧠 Lesson 05: Memory & Performance

## 🎯 Goal

Master WASM memory management and write high-performance code.

## 📊 WASM Memory Model

```
┌─────────────────────────────────────────────────────────────┐
│                    WASM Linear Memory                       │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ 0x0000                                        0xFFFF │   │
│  │ ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐ │   │
│  │ │    │    │    │    │    │    │    │    │    │    │ │   │
│  │ └────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘ │   │
│  │  ↑                                                    │   │
│  │  One contiguous array of bytes                        │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                             │
│  • Grows in 64KB pages                                      │
│  • Both JS and WASM can read/write                          │
│  • No automatic garbage collection                          │
└─────────────────────────────────────────────────────────────┘
```

## 🔑 Key Concepts

### 1. Linear Memory

WASM has one contiguous block of memory:

- Starts at 0
- Grows in 64KB pages
- Accessed by index (like an array)

```javascript
// Create memory (1 page = 64KB)
const memory = new WebAssembly.Memory({ initial: 1, maximum: 10 });

// Access as different typed arrays
const bytes = new Uint8Array(memory.buffer);
const ints = new Int32Array(memory.buffer);
const floats = new Float64Array(memory.buffer);
```

### 2. No Garbage Collection

WASM doesn't have GC. You manage memory:

- Allocate when needed
- Free when done
- Or use Rust/wasm-bindgen (handles this for you)

### 3. Memory Alignment

For best performance, align data:

- i32: 4-byte boundary
- i64/f64: 8-byte boundary

## 📝 Memory Operations in WAT

```wat
(module
  ;; Declare memory (1 page minimum)
  (memory 1)

  ;; Store i32 at address 0
  (func $store_int (param $value i32)
    i32.const 0        ;; address
    local.get $value   ;; value
    i32.store          ;; store 4 bytes
  )

  ;; Load i32 from address 0
  (func $load_int (result i32)
    i32.const 0        ;; address
    i32.load           ;; load 4 bytes
  )

  ;; Store single byte
  (func $store_byte (param $addr i32) (param $value i32)
    local.get $addr
    local.get $value
    i32.store8         ;; store 1 byte
  )
)
```

## 🏎️ Performance Patterns

### Pattern 1: Batch Processing

**Bad:** Many small JS↔WASM calls

```javascript
// ❌ Slow: 1000 function calls
for (let i = 0; i < 1000; i++) {
	wasm.processItem(items[i]);
}
```

**Good:** Pass array, process in WASM

```javascript
// ✅ Fast: 1 function call
const ptr = copyToMemory(items);
wasm.processItems(ptr, items.length);
```

### Pattern 2: Avoid Memory Copies

```javascript
// ❌ Slow: Copy data twice
const data = new Float64Array([1, 2, 3, 4, 5]);
const wasmData = wasm.allocate(data.length * 8);
for (let i = 0; i < data.length; i++) {
	wasm.setFloat(wasmData + i * 8, data[i]);
}

// ✅ Fast: Direct memory access
const memory = new Float64Array(wasm.memory.buffer);
memory.set(data, wasmPtr / 8);
```

### Pattern 3: Struct Packing

```c
// ❌ Wasteful: may have padding
struct Bad {
    char a;     // 1 byte + 7 padding
    double b;   // 8 bytes
    char c;     // 1 byte + 7 padding
};

// ✅ Efficient: packed structure
#pragma pack(push, 1)
struct Good {
    double b;   // 8 bytes (aligned)
    char a;     // 1 byte
    char c;     // 1 byte
};
#pragma pack(pop)
```

## 📊 Performance Benchmarks

| Operation          | JS Time | WASM Time | Speedup |
| ------------------ | ------- | --------- | ------- |
| Fibonacci(40)      | 1200ms  | 450ms     | 2.7x    |
| Matrix 1000x1000   | 500ms   | 120ms     | 4.2x    |
| Image blur 4K      | 800ms   | 150ms     | 5.3x    |
| JSON parse (large) | 100ms   | 95ms      | 1.05x   |

**Key insight:** WASM shines for computation, not I/O.

## 🧪 When to Use WASM

### ✅ Use WASM For:

- Heavy computation (math, crypto)
- Image/video processing
- Physics simulations
- Game logic
- Parsing binary formats
- Compression/decompression

### ❌ Don't Use WASM For:

- DOM manipulation
- Simple CRUD apps
- String-heavy operations
- I/O bound tasks
- Small computations (call overhead)

## 💻 Practical Example: Image Processing

### C Code

```c
#include <emscripten.h>
#include <stdint.h>

EMSCRIPTEN_KEEPALIVE
void grayscale(uint8_t* pixels, int width, int height) {
    int size = width * height * 4;
    for (int i = 0; i < size; i += 4) {
        float r = (float)pixels[i];
        float g = (float)pixels[i + 1];
        float b = (float)pixels[i + 2];
        
        // Luminance formula
        uint8_t gray = (uint8_t)(0.299f * r + 0.587f * g + 0.114f * b);
        
        pixels[i] = gray;
        pixels[i + 1] = gray;
        pixels[i + 2] = gray;
        // Alpha (i + 3) unchanged
    }
}
```

### JavaScript Usage

```javascript
// Get image data
const ctx = canvas.getContext("2d");
const imageData = ctx.getImageData(0, 0, width, height);

// Copy to WASM memory
const ptr = Module._malloc(imageData.data.length);
const wasmMem = new Uint8Array(Module.HEAPU8.buffer);
wasmMem.set(imageData.data, ptr);

// Process in WASM
Module._grayscale(ptr, width, height);

// Copy back and display
imageData.data.set(wasmMem.slice(ptr, ptr + imageData.data.length));
ctx.putImageData(imageData, 0, 0);

// Free memory
Module._free(ptr);
```

## 🔧 Profiling Tools

1. **Chrome DevTools** → Performance tab
2. **Firefox Profiler** → Shows WASM functions
3. **`console.time()`** → Quick benchmarks
4. **C `inline`** → Reduce call overhead
5. **Emscripten `-O3`** → Maximum optimization

## ✅ What You Learned

1. WASM uses linear memory (one big array)
2. Memory grows in 64KB pages
3. Batch operations beat many small calls
4. WASM excels at computation, not I/O
5. Profile before optimizing!

## ➡️ Next Step

🎉 **Congratulations!** You've completed the WASM fundamentals.

Now apply what you learned to the **mix** project!

[Go to mix project →](../../mix/README.md)

---

💡 **Final Tip:** Don't over-optimize early. Profile first, then optimize the hot paths.
