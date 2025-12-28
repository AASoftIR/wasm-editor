# 🌉 Lesson 03: WASM + JavaScript Bridge

## 🎯 Goal

Learn how JavaScript and WebAssembly talk to each other.

This is **the most important lesson** for web developers!

## 🔗 The Bridge Concept

```
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│   JavaScript World              WASM World                  │
│   ┌─────────────┐              ┌─────────────┐              │
│   │ • Strings   │              │ • Numbers   │              │
│   │ • Objects   │    Bridge    │ • i32, i64  │              │
│   │ • Arrays    │◄────────────►│ • f32, f64  │              │
│   │ • Functions │              │ • Memory    │              │
│   │ • DOM       │              │ • Functions │              │
│   └─────────────┘              └─────────────┘              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## ⚠️ The Limitation

WASM can only directly pass **numbers**:

- `i32` - 32-bit integer
- `i64` - 64-bit integer
- `f32` - 32-bit float
- `f64` - 64-bit float

**No strings, objects, or arrays directly!**

## 🎮 Three Communication Patterns

### Pattern 1: Simple Numbers (Easy!)

```javascript
// JS calls WASM
const result = wasm.add(5, 3); // Returns 8

// WASM function signature:
// (func $add (param i32 i32) (result i32))
```

### Pattern 2: Shared Memory (Strings & Arrays)

```javascript
// Create shared memory
const memory = new WebAssembly.Memory({ initial: 1 });

// Write string to memory
const encoder = new TextEncoder();
const bytes = encoder.encode("Hello WASM!");
new Uint8Array(memory.buffer).set(bytes, 0);

// Tell WASM where to find it
wasm.processString(0, bytes.length); // pointer, length
```

### Pattern 3: Import Functions (WASM calls JS!)

```javascript
const imports = {
	env: {
		// WASM can call this JS function!
		log: (value) => console.log("From WASM:", value),

		// Let WASM manipulate DOM
		updateScore: (score) => {
			document.getElementById("score").textContent = score;
		},
	},
};

const { instance } = await WebAssembly.instantiate(wasmBytes, imports);
```

## 📝 Complete Example: String Processing

### The WASM Part (string.wat)

```wat
(module
  ;; Import memory from JavaScript
  (import "env" "memory" (memory 1))

  ;; Import console.log from JavaScript
  (import "env" "log" (func $log (param i32)))

  ;; Count uppercase letters in string
  ;; Parameters: pointer to string, length
  ;; Returns: count of uppercase letters
  (func $countUppercase (param $ptr i32) (param $len i32) (result i32)
    (local $i i32)
    (local $count i32)
    (local $char i32)

    ;; Loop through each character
    (block $break
      (loop $continue
        ;; Check if done
        local.get $i
        local.get $len
        i32.ge_u
        br_if $break

        ;; Load character from memory
        local.get $ptr
        local.get $i
        i32.add
        i32.load8_u
        local.set $char

        ;; Check if uppercase (65-90 = A-Z)
        local.get $char
        i32.const 65
        i32.ge_u
        local.get $char
        i32.const 90
        i32.le_u
        i32.and

        ;; Increment count if uppercase
        (if
          (then
            local.get $count
            i32.const 1
            i32.add
            local.set $count
          )
        )

        ;; i++
        local.get $i
        i32.const 1
        i32.add
        local.set $i

        br $continue
      )
    )

    local.get $count
  )

  (export "countUppercase" (func $countUppercase))
)
```

### The JavaScript Part

```javascript
async function init() {
	// Shared memory
	const memory = new WebAssembly.Memory({ initial: 1 });

	// Imports object
	const imports = {
		env: {
			memory,
			log: (val) => console.log("WASM says:", val),
		},
	};

	// Load WASM
	const response = await fetch("string.wasm");
	const bytes = await response.arrayBuffer();
	const { instance } = await WebAssembly.instantiate(bytes, imports);

	// Write string to shared memory
	const text = "Hello WASM World!";
	const encoder = new TextEncoder();
	const encoded = encoder.encode(text);
	new Uint8Array(memory.buffer).set(encoded, 0);

	// Call WASM function
	const count = instance.exports.countUppercase(0, encoded.length);
	console.log(`Uppercase letters in "${text}": ${count}`);
	// Output: Uppercase letters in "Hello WASM World!": 3
}
```

## 🧠 Memory Layout

```
Memory Buffer (ArrayBuffer)
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│  H  │  e  │  l  │  l  │  o  │     │  W  │  A  │ ... │
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│  0  │  1  │  2  │  3  │  4  │  5  │  6  │  7  │ ... │
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
  ↑
  pointer = 0
```

## 🎮 Try It!

Open `demo.html` to see:

1. Numbers crossing the bridge
2. Strings via shared memory
3. WASM calling JavaScript functions

## ✅ Key Takeaways

1. **Numbers are easy** — pass directly
2. **Strings need memory** — encode to bytes, share pointer + length
3. **WASM can call JS** — via imported functions
4. **Memory is shared** — both sides can read/write

## 🚨 Common Pitfalls

| Pitfall                                | Solution                          |
| -------------------------------------- | --------------------------------- |
| Forgetting to pass length with strings | Always pass (pointer, length)     |
| Memory buffer detached after grow      | Re-get `memory.buffer` after grow |
| Trying to pass objects                 | Serialize to JSON bytes first     |
| 64-bit integers in JS                  | Use BigInt for i64                |

## ➡️ Next Lesson

[04 - C → WASM (The Real Power) →](../04-c-wasm/README.md)

---

💡 **Pro Tip:** Tools like Emscripten automate all this bridging for you. But understanding the fundamentals helps when debugging!
