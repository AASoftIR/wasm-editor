#!/bin/bash

echo ""
echo "========================================"
echo "  Building C to WASM with Emscripten"
echo "========================================"
echo ""

# Check for emcc
if ! command -v emcc &> /dev/null; then
    echo "ERROR: Emscripten not found!"
    echo ""
    echo "Install Emscripten:"
    echo "  macOS:  brew install emscripten"
    echo "  Linux:  git clone https://github.com/emscripten-core/emsdk.git"
    echo "          cd emsdk && ./emsdk install latest && ./emsdk activate latest"
    echo "          source ./emsdk_env.sh"
    exit 1
fi

echo "[1/2] Compiling C to WASM..."

emcc src/main.c -o www/main.js \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS="['_add', '_subtract', '_multiply', '_divide', '_fibonacci', '_fibonacci_fast', '_factorial', '_sum_array', '_average_array', '_find_max', '_find_min', '_greet', '_string_length', '_reverse_string', '_allocate_memory', '_free_memory', '_create_int_array', '_create_double_array', '_heavy_computation', '_log_to_console', '_get_random_int', '_get_current_time', '_main', '_malloc', '_free']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'UTF8ToString', 'stringToUTF8', 'getValue', 'setValue', 'HEAP8', 'HEAP16', 'HEAP32', 'HEAPF32', 'HEAPF64']" \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=0 \
    -O2

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: Build failed!"
    exit 1
fi

echo "[2/2] Build complete!"
echo ""
echo "Output files:"
echo "  - www/main.js"
echo "  - www/main.wasm"
echo ""
echo "To test, start a local server:"
echo "  python3 -m http.server 8080 --directory www"
echo "  Then open http://localhost:8080"
echo ""
