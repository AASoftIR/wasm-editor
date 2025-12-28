// main.c - WASM Module compiled from C
// Build with: emcc main.c -o ../www/main.js -s WASM=1 -s EXPORTED_FUNCTIONS="[...]" -O2

#include <emscripten.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// ═══════════════════════════════════════════════════════════════════════════
// BASIC MATH FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
int add(int a, int b) {
    return a + b;
}

EMSCRIPTEN_KEEPALIVE
int subtract(int a, int b) {
    return a - b;
}

EMSCRIPTEN_KEEPALIVE
int multiply(int a, int b) {
    return a * b;
}

EMSCRIPTEN_KEEPALIVE
double divide(double a, double b) {
    if (b == 0) return 0;
    return a / b;
}

// ═══════════════════════════════════════════════════════════════════════════
// FIBONACCI - The classic WASM benchmark
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// Iterative version (much faster)
EMSCRIPTEN_KEEPALIVE
int fibonacci_fast(int n) {
    if (n <= 1) return n;
    
    int prev = 0, curr = 1;
    for (int i = 2; i <= n; i++) {
        int next = prev + curr;
        prev = curr;
        curr = next;
    }
    return curr;
}

// ═══════════════════════════════════════════════════════════════════════════
// FACTORIAL
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
long long factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// ═══════════════════════════════════════════════════════════════════════════
// ARRAY OPERATIONS
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
int sum_array(int* arr, int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

EMSCRIPTEN_KEEPALIVE
double average_array(double* arr, int length) {
    if (length == 0) return 0;
    
    double sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum / length;
}

EMSCRIPTEN_KEEPALIVE
int find_max(int* arr, int length) {
    if (length == 0) return 0;
    
    int max = arr[0];
    for (int i = 1; i < length; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}

EMSCRIPTEN_KEEPALIVE
int find_min(int* arr, int length) {
    if (length == 0) return 0;
    
    int min = arr[0];
    for (int i = 1; i < length; i++) {
        if (arr[i] < min) min = arr[i];
    }
    return min;
}

// ═══════════════════════════════════════════════════════════════════════════
// STRING OPERATIONS
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
char* greet(const char* name) {
    static char buffer[512];
    snprintf(buffer, sizeof(buffer), "Hello, %s! 👋 from C/WASM", name);
    return buffer;
}

EMSCRIPTEN_KEEPALIVE
int string_length(const char* str) {
    return strlen(str);
}

EMSCRIPTEN_KEEPALIVE
char* reverse_string(const char* str) {
    static char buffer[1024];
    int len = strlen(str);
    
    for (int i = 0; i < len; i++) {
        buffer[i] = str[len - 1 - i];
    }
    buffer[len] = '\0';
    
    return buffer;
}

// ═══════════════════════════════════════════════════════════════════════════
// MEMORY MANAGEMENT HELPERS
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
void* allocate_memory(int size) {
    return malloc(size);
}

EMSCRIPTEN_KEEPALIVE
void free_memory(void* ptr) {
    free(ptr);
}

EMSCRIPTEN_KEEPALIVE
int* create_int_array(int size) {
    return (int*)malloc(size * sizeof(int));
}

EMSCRIPTEN_KEEPALIVE
double* create_double_array(int size) {
    return (double*)malloc(size * sizeof(double));
}

// ═══════════════════════════════════════════════════════════════════════════
// PERFORMANCE BENCHMARK FUNCTION
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
int heavy_computation(int iterations) {
    int result = 0;
    for (int i = 0; i < iterations; i++) {
        result += (i * i) % 1000;
        result ^= (result << 3);
        result = (result + 17) % 10000;
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
// CALLING JAVASCRIPT FROM C (Advanced)
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
void log_to_console(const char* message) {
    EM_ASM({
        console.log('[C/WASM]:', UTF8ToString($0));
    }, message);
}

EMSCRIPTEN_KEEPALIVE
int get_random_int(int max) {
    return EM_ASM_INT({
        return Math.floor(Math.random() * $0);
    }, max);
}

EMSCRIPTEN_KEEPALIVE
double get_current_time() {
    return EM_ASM_DOUBLE({
        return performance.now();
    });
}

// ═══════════════════════════════════════════════════════════════════════════
// MAIN (Optional - called on module initialization)
// ═══════════════════════════════════════════════════════════════════════════

int main() {
    EM_ASM({
        console.log('✅ C/WASM Module Initialized!');
    });
    return 0;
}
