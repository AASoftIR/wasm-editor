;; add.wat - WebAssembly Text Format
;; This is the human-readable form of WebAssembly
;; 
;; To compile: wat2wasm add.wat -o add.wasm
;; (requires wabt toolkit)

(module
  ;; Define a function named "add"
  ;; Takes two 32-bit integers (i32) as parameters
  ;; Returns one 32-bit integer
  (func $add (param $a i32) (param $b i32) (result i32)
    ;; Push first parameter onto stack
    local.get $a
    ;; Push second parameter onto stack
    local.get $b
    ;; Add them (pops two, pushes result)
    i32.add
  )
  
  ;; Export the function so JavaScript can call it
  (export "add" (func $add))
)

;; ═══════════════════════════════════════════════════════════
;; WHAT'S HAPPENING HERE?
;; ═══════════════════════════════════════════════════════════
;;
;; WASM uses a STACK-BASED virtual machine:
;;
;;   1. local.get $a    →  Stack: [5]
;;   2. local.get $b    →  Stack: [5, 7]
;;   3. i32.add         →  Stack: [12]  (pops 5 and 7, pushes 12)
;;
;; The final value on the stack is the return value!
;;
;; ═══════════════════════════════════════════════════════════
;; DATA TYPES IN WASM
;; ═══════════════════════════════════════════════════════════
;;
;;   i32  - 32-bit integer
;;   i64  - 64-bit integer  
;;   f32  - 32-bit float
;;   f64  - 64-bit float
;;
;; That's it! Only 4 numeric types. Simple!
;;
;; ═══════════════════════════════════════════════════════════
