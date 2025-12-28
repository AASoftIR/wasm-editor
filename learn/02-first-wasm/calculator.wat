;; calculator.wat - A Complete WASM Calculator
;; 
;; Compile with: wat2wasm calculator.wat -o calculator.wasm
;; Or use online: https://webassembly.github.io/wabt/demo/wat2wasm/

(module
  ;; ═══════════════════════════════════════════════════════════
  ;; BASIC ARITHMETIC OPERATIONS
  ;; All take two i32 parameters and return i32
  ;; ═══════════════════════════════════════════════════════════
  
  ;; Addition: a + b
  (func $add (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.add
  )
  
  ;; Subtraction: a - b
  (func $subtract (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.sub
  )
  
  ;; Multiplication: a × b
  (func $multiply (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.mul
  )
  
  ;; Division: a ÷ b (signed integer division)
  (func $divide (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.div_s  ;; _s means "signed"
  )
  
  ;; ═══════════════════════════════════════════════════════════
  ;; ADVANCED: Factorial using recursion
  ;; Demonstrates: conditionals, recursion, local variables
  ;; ═══════════════════════════════════════════════════════════
  
  (func $factorial (param $n i32) (result i32)
    ;; Base case: if n <= 1, return 1
    local.get $n
    i32.const 1
    i32.le_s              ;; n <= 1 ?
    
    (if (result i32)
      (then
        i32.const 1       ;; return 1
      )
      (else
        ;; Recursive case: n * factorial(n - 1)
        local.get $n
        
        local.get $n
        i32.const 1
        i32.sub           ;; n - 1
        call $factorial   ;; factorial(n - 1)
        
        i32.mul           ;; n * factorial(n - 1)
      )
    )
  )
  
  ;; ═══════════════════════════════════════════════════════════
  ;; EXPORTS - Make functions available to JavaScript
  ;; ═══════════════════════════════════════════════════════════
  
  (export "add" (func $add))
  (export "subtract" (func $subtract))
  (export "multiply" (func $multiply))
  (export "divide" (func $divide))
  (export "factorial" (func $factorial))
)

;; ═══════════════════════════════════════════════════════════
;; HOW TO USE IN JAVASCRIPT:
;; ═══════════════════════════════════════════════════════════
;;
;; const response = await fetch('calculator.wasm');
;; const bytes = await response.arrayBuffer();
;; const { instance } = await WebAssembly.instantiate(bytes);
;;
;; instance.exports.add(10, 5)        // → 15
;; instance.exports.subtract(10, 5)  // → 5  
;; instance.exports.multiply(10, 5)  // → 50
;; instance.exports.divide(10, 5)    // → 2
;; instance.exports.factorial(5)     // → 120
;;
;; ═══════════════════════════════════════════════════════════
