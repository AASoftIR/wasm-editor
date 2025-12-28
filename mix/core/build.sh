#!/bin/bash

echo ""
echo "========================================"
echo "  Building mix-core (C to WASM)"
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

mkdir -p ../public

echo "[1/2] Compiling C to WASM..."

emcc src/main.c src/buffer.c \
    -o ../public/mix-core.js \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS="['_editor_init', '_editor_load_text', '_editor_destroy', '_get_buffer_length', '_get_line_count', '_get_all_text', '_get_line', '_insert_text', '_delete_text', '_get_cursor_position', '_get_cursor_line', '_get_cursor_column', '_set_cursor_position', '_get_mode', '_set_mode', '_get_mode_name', '_motion_h', '_motion_l', '_motion_j', '_motion_k', '_motion_w', '_motion_b', '_motion_e', '_motion_line_start', '_motion_line_end', '_motion_file_start', '_motion_file_end', '_insert_char', '_insert_string', '_delete_char_before', '_delete_char_after', '_delete_line', '_has_selection', '_get_selection_start', '_get_selection_end', '_set_search_pattern', '_search_next', '_search_prev', '_free_string', '_malloc', '_free', '_main']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'UTF8ToString', 'stringToUTF8', 'getValue', 'setValue']" \
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
echo "  - public/mix-core.js"
echo "  - public/mix-core.wasm"
echo ""
