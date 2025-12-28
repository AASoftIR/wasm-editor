# 🎮 Lesson 02: Your First WASM Module

## 🎯 Goal

Create and run a WASM module **without installing Rust or any compiler!**

We'll write WASM by hand using **WAT (WebAssembly Text Format)** and convert it online.

## 📦 What You'll Build

A simple calculator that runs entirely in WASM:

- Add
- Subtract
- Multiply
- Factorial (recursive!)

## 🛠️ Two Ways to Create WASM

### Option A: Online Compiler (Easiest!)

1. Go to [WebAssembly Studio](https://webassembly.studio/) or [WAT2WASM Online](https://webassembly.github.io/wabt/demo/wat2wasm/)
2. Paste WAT code
3. Download `.wasm` file

### Option B: Install WABT Tools

```bash
# macOS
brew install wabt

# Ubuntu/Debian
apt-get install wabt

# Windows (via npm)
npm install -g wabt
```

Then compile:

```bash
wat2wasm calculator.wat -o calculator.wasm
```

## 📝 The Code

### calculator.wat

```wat
(module
  ;; ═══════════════════════════════════════
  ;; BASIC MATH OPERATIONS
  ;; ═══════════════════════════════════════

  ;; Add two numbers
  (func $add (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.add
  )

  ;; Subtract: a - b
  (func $subtract (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.sub
  )

  ;; Multiply
  (func $multiply (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.mul
  )

  ;; ═══════════════════════════════════════
  ;; ADVANCED: Factorial (recursive!)
  ;; ═══════════════════════════════════════

  (func $factorial (param $n i32) (result i32)
    ;; if (n <= 1) return 1
    local.get $n
    i32.const 1
    i32.le_s              ;; less than or equal (signed)
    if (result i32)
      i32.const 1         ;; return 1
    else
      ;; return n * factorial(n - 1)
      local.get $n
      local.get $n
      i32.const 1
      i32.sub
      call $factorial     ;; recursive call!
      i32.mul
    end
  )

  ;; Export all functions
  (export "add" (func $add))
  (export "subtract" (func $subtract))
  (export "multiply" (func $multiply))
  (export "factorial" (func $factorial))
)
```

## 🔑 Key Concepts

### 1. Stack-Based Operations

```
                    Stack
local.get $a    →   [10]
local.get $b    →   [10, 3]
i32.sub         →   [7]        ← Result!
```

### 2. Control Flow

```wat
if (result i32)    ;; if block returns i32
  i32.const 1      ;; true branch
else
  ...              ;; false branch
end
```

### 3. Recursive Calls

```wat
call $factorial    ;; Call another WASM function
```

## 🌐 Loading WASM in JavaScript

```javascript
// Method 1: Fetch and instantiate
async function loadWasm() {
  const response = await fetch('calculator.wasm');
  const bytes = await response.arrayBuffer();
  const { instance } = await WebAssembly.instantiate(bytes);

  // Now use it!
  console.log(instance.exports.add(5, 3));        // 8
  console.log(instance.exports.factorial(5));     // 120
}

// Method 2: Inline binary (no server needed!)
const wasmBinary = new Uint8Array([0x00, 0x61, ...]);
const { instance } = await WebAssembly.instantiate(wasmBinary);
```

## 🎮 Try It!

Open `demo.html` in your browser — no server needed!

```
📁 02-first-wasm/
├── README.md           ← You are here
├── demo.html           ← Interactive demo
├── calculator.wat      ← Human-readable source
└── calculator.wasm     ← Compiled binary (pre-built)
```

## 💡 WAT Cheatsheet

| Operation    | WAT         | Description    |
| ------------ | ----------- | -------------- |
| Add          | `i32.add`   | a + b          |
| Subtract     | `i32.sub`   | a - b          |
| Multiply     | `i32.mul`   | a × b          |
| Divide       | `i32.div_s` | a ÷ b (signed) |
| Remainder    | `i32.rem_s` | a % b          |
| Equal        | `i32.eq`    | a == b         |
| Less than    | `i32.lt_s`  | a < b          |
| Greater than | `i32.gt_s`  | a > b          |

## ✅ What You Learned

1. WAT is human-readable WebAssembly
2. WASM uses a stack-based execution model
3. You can write recursive functions in WASM
4. Loading WASM in JS is straightforward

## ➡️ Next Lesson

[03 - WASM + JavaScript Bridge →](../03-js-bridge/README.md)

---

💡 **Fun Fact:** The entire WASM spec is only ~4 numeric types and ~170 instructions. Compare that to x86's thousands of instructions!
