// main.c - Mix Editor WASM Entry Point
// Build with Emscripten: emcc main.c buffer.c cursor.c vim.c undo.c -o mix-core.js ...

#include <emscripten.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "buffer.h"

// ═══════════════════════════════════════════════════════════════════════════
// GLOBAL STATE
// ═══════════════════════════════════════════════════════════════════════════

static Buffer* g_buffer = NULL;

typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_VISUAL,
    MODE_COMMAND,  // : command line
    MODE_SEARCH    // / search
} EditorMode;

typedef struct {
    size_t position;    // Absolute position in buffer
    size_t line;        // Current line (0-indexed)
    size_t column;      // Current column (0-indexed)
    size_t preferred_column; // For vertical movement
} Cursor;

static Cursor g_cursor = {0, 0, 0, 0};
static EditorMode g_mode = MODE_NORMAL;

// Selection for visual mode
static size_t g_selection_start = 0;
static bool g_has_selection = false;

// ═══════════════════════════════════════════════════════════════════════════
// INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
int editor_init(void) {
    if (g_buffer) {
        buffer_destroy(g_buffer);
    }
    g_buffer = buffer_create("");
    g_cursor = (Cursor){0, 0, 0, 0};
    g_mode = MODE_NORMAL;
    g_has_selection = false;
    
    EM_ASM({
        console.log('[mix-core] Editor initialized');
    });
    
    return g_buffer != NULL;
}

EMSCRIPTEN_KEEPALIVE
int editor_load_text(const char* text) {
    if (g_buffer) {
        buffer_destroy(g_buffer);
    }
    g_buffer = buffer_create(text);
    g_cursor = (Cursor){0, 0, 0, 0};
    return g_buffer != NULL;
}

EMSCRIPTEN_KEEPALIVE
void editor_destroy(void) {
    if (g_buffer) {
        buffer_destroy(g_buffer);
        g_buffer = NULL;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// BUFFER OPERATIONS (Exported to JS)
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
size_t get_buffer_length(void) {
    return g_buffer ? buffer_length(g_buffer) : 0;
}

EMSCRIPTEN_KEEPALIVE
size_t get_line_count(void) {
    return g_buffer ? buffer_line_count(g_buffer) : 0;
}

EMSCRIPTEN_KEEPALIVE
char* get_all_text(void) {
    return g_buffer ? buffer_get_all(g_buffer) : NULL;
}

EMSCRIPTEN_KEEPALIVE
char* get_line(size_t line_number) {
    return g_buffer ? buffer_get_line(g_buffer, line_number) : NULL;
}

EMSCRIPTEN_KEEPALIVE
int insert_text(size_t position, const char* text) {
    if (!g_buffer || !text) return 0;
    return buffer_insert(g_buffer, position, text, strlen(text));
}

EMSCRIPTEN_KEEPALIVE
int delete_text(size_t position, size_t length) {
    if (!g_buffer) return 0;
    return buffer_delete(g_buffer, position, length);
}

// ═══════════════════════════════════════════════════════════════════════════
// CURSOR OPERATIONS
// ═══════════════════════════════════════════════════════════════════════════

static void update_cursor_line_col(void) {
    if (!g_buffer) return;
    buffer_pos_to_line_col(g_buffer, g_cursor.position, &g_cursor.line, &g_cursor.column);
}

static void clamp_cursor(void) {
    if (!g_buffer) return;
    
    size_t len = buffer_length(g_buffer);
    if (g_cursor.position > len) {
        g_cursor.position = len;
    }
    update_cursor_line_col();
}

EMSCRIPTEN_KEEPALIVE
size_t get_cursor_position(void) {
    return g_cursor.position;
}

EMSCRIPTEN_KEEPALIVE
size_t get_cursor_line(void) {
    return g_cursor.line;
}

EMSCRIPTEN_KEEPALIVE
size_t get_cursor_column(void) {
    return g_cursor.column;
}

EMSCRIPTEN_KEEPALIVE
void set_cursor_position(size_t position) {
    g_cursor.position = position;
    clamp_cursor();
    g_cursor.preferred_column = g_cursor.column;
}

// ═══════════════════════════════════════════════════════════════════════════
// MODE MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
int get_mode(void) {
    return (int)g_mode;
}

EMSCRIPTEN_KEEPALIVE
void set_mode(int mode) {
    g_mode = (EditorMode)mode;
    
    if (mode == MODE_VISUAL) {
        g_selection_start = g_cursor.position;
        g_has_selection = true;
    } else if (mode == MODE_NORMAL) {
        g_has_selection = false;
    }
}

EMSCRIPTEN_KEEPALIVE
const char* get_mode_name(void) {
    switch (g_mode) {
        case MODE_NORMAL: return "NORMAL";
        case MODE_INSERT: return "INSERT";
        case MODE_VISUAL: return "VISUAL";
        case MODE_COMMAND: return "COMMAND";
        case MODE_SEARCH: return "SEARCH";
        default: return "UNKNOWN";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// VIM MOTIONS
// ═══════════════════════════════════════════════════════════════════════════

// h - move left
EMSCRIPTEN_KEEPALIVE
void motion_h(void) {
    if (g_cursor.position > 0) {
        g_cursor.position--;
        update_cursor_line_col();
        g_cursor.preferred_column = g_cursor.column;
    }
}

// l - move right
EMSCRIPTEN_KEEPALIVE
void motion_l(void) {
    if (g_buffer && g_cursor.position < buffer_length(g_buffer)) {
        g_cursor.position++;
        update_cursor_line_col();
        g_cursor.preferred_column = g_cursor.column;
    }
}

// j - move down
EMSCRIPTEN_KEEPALIVE
void motion_j(void) {
    if (!g_buffer) return;
    
    size_t line_count = buffer_line_count(g_buffer);
    if (g_cursor.line + 1 >= line_count) return;
    
    size_t next_line_start = buffer_line_start(g_buffer, g_cursor.line + 1);
    size_t next_line_len = buffer_line_length(g_buffer, g_cursor.line + 1);
    
    // Remove newline from length calculation
    if (next_line_len > 0 && buffer_char_at(g_buffer, next_line_start + next_line_len - 1) == '\n') {
        next_line_len--;
    }
    
    size_t new_col = g_cursor.preferred_column;
    if (new_col > next_line_len) new_col = next_line_len;
    
    g_cursor.position = next_line_start + new_col;
    update_cursor_line_col();
}

// k - move up
EMSCRIPTEN_KEEPALIVE
void motion_k(void) {
    if (!g_buffer || g_cursor.line == 0) return;
    
    size_t prev_line_start = buffer_line_start(g_buffer, g_cursor.line - 1);
    size_t prev_line_len = buffer_line_length(g_buffer, g_cursor.line - 1);
    
    // Remove newline from length calculation
    if (prev_line_len > 0 && buffer_char_at(g_buffer, prev_line_start + prev_line_len - 1) == '\n') {
        prev_line_len--;
    }
    
    size_t new_col = g_cursor.preferred_column;
    if (new_col > prev_line_len) new_col = prev_line_len;
    
    g_cursor.position = prev_line_start + new_col;
    update_cursor_line_col();
}

// w - move to next word
EMSCRIPTEN_KEEPALIVE
void motion_w(void) {
    if (!g_buffer) return;
    
    size_t len = buffer_length(g_buffer);
    size_t pos = g_cursor.position;
    
    // Skip current word
    while (pos < len) {
        char c = buffer_char_at(g_buffer, pos);
        if (c == ' ' || c == '\t' || c == '\n') break;
        pos++;
    }
    
    // Skip whitespace
    while (pos < len) {
        char c = buffer_char_at(g_buffer, pos);
        if (c != ' ' && c != '\t' && c != '\n') break;
        pos++;
    }
    
    g_cursor.position = pos;
    update_cursor_line_col();
    g_cursor.preferred_column = g_cursor.column;
}

// b - move to previous word
EMSCRIPTEN_KEEPALIVE
void motion_b(void) {
    if (!g_buffer || g_cursor.position == 0) return;
    
    size_t pos = g_cursor.position - 1;
    
    // Skip whitespace
    while (pos > 0) {
        char c = buffer_char_at(g_buffer, pos);
        if (c != ' ' && c != '\t' && c != '\n') break;
        pos--;
    }
    
    // Find word start
    while (pos > 0) {
        char c = buffer_char_at(g_buffer, pos - 1);
        if (c == ' ' || c == '\t' || c == '\n') break;
        pos--;
    }
    
    g_cursor.position = pos;
    update_cursor_line_col();
    g_cursor.preferred_column = g_cursor.column;
}

// e - move to end of word
EMSCRIPTEN_KEEPALIVE
void motion_e(void) {
    if (!g_buffer) return;
    
    size_t len = buffer_length(g_buffer);
    size_t pos = g_cursor.position;
    
    // Move at least one character
    if (pos < len) pos++;
    
    // Skip whitespace
    while (pos < len) {
        char c = buffer_char_at(g_buffer, pos);
        if (c != ' ' && c != '\t' && c != '\n') break;
        pos++;
    }
    
    // Move to end of word
    while (pos < len - 1) {
        char c = buffer_char_at(g_buffer, pos + 1);
        if (c == ' ' || c == '\t' || c == '\n') break;
        pos++;
    }
    
    g_cursor.position = pos;
    update_cursor_line_col();
    g_cursor.preferred_column = g_cursor.column;
}

// 0 or ^ - move to start of line
EMSCRIPTEN_KEEPALIVE
void motion_line_start(void) {
    if (!g_buffer) return;
    g_cursor.position = buffer_line_start(g_buffer, g_cursor.line);
    update_cursor_line_col();
    g_cursor.preferred_column = 0;
}

// $ - move to end of line
EMSCRIPTEN_KEEPALIVE
void motion_line_end(void) {
    if (!g_buffer) return;
    
    size_t line_start = buffer_line_start(g_buffer, g_cursor.line);
    size_t line_len = buffer_line_length(g_buffer, g_cursor.line);
    
    // Don't include newline
    if (line_len > 0 && buffer_char_at(g_buffer, line_start + line_len - 1) == '\n') {
        line_len--;
    }
    
    g_cursor.position = line_start + line_len;
    update_cursor_line_col();
    g_cursor.preferred_column = g_cursor.column;
}

// gg - go to first line
EMSCRIPTEN_KEEPALIVE
void motion_file_start(void) {
    g_cursor.position = 0;
    update_cursor_line_col();
    g_cursor.preferred_column = 0;
}

// G - go to last line
EMSCRIPTEN_KEEPALIVE
void motion_file_end(void) {
    if (!g_buffer) return;
    
    size_t line_count = buffer_line_count(g_buffer);
    if (line_count > 0) {
        g_cursor.position = buffer_line_start(g_buffer, line_count - 1);
    }
    update_cursor_line_col();
    g_cursor.preferred_column = g_cursor.column;
}

// ═══════════════════════════════════════════════════════════════════════════
// INSERT OPERATIONS
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
int insert_char(char c) {
    if (!g_buffer) return 0;
    
    char str[2] = {c, '\0'};
    if (buffer_insert(g_buffer, g_cursor.position, str, 1)) {
        g_cursor.position++;
        update_cursor_line_col();
        return 1;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int insert_string(const char* str) {
    if (!g_buffer || !str) return 0;
    
    size_t len = strlen(str);
    if (buffer_insert(g_buffer, g_cursor.position, str, len)) {
        g_cursor.position += len;
        update_cursor_line_col();
        return 1;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int delete_char_before(void) {
    if (!g_buffer || g_cursor.position == 0) return 0;
    
    if (buffer_delete(g_buffer, g_cursor.position - 1, 1)) {
        g_cursor.position--;
        update_cursor_line_col();
        return 1;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int delete_char_after(void) {
    if (!g_buffer) return 0;
    
    if (g_cursor.position < buffer_length(g_buffer)) {
        return buffer_delete(g_buffer, g_cursor.position, 1);
    }
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// VIM OPERATORS
// ═══════════════════════════════════════════════════════════════════════════

// dd - delete line
EMSCRIPTEN_KEEPALIVE
int delete_line(void) {
    if (!g_buffer) return 0;
    
    size_t line_start = buffer_line_start(g_buffer, g_cursor.line);
    size_t line_len = buffer_line_length(g_buffer, g_cursor.line);
    
    if (buffer_delete(g_buffer, line_start, line_len)) {
        g_cursor.position = line_start;
        clamp_cursor();
        return 1;
    }
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// SELECTION
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
int has_selection(void) {
    return g_has_selection;
}

EMSCRIPTEN_KEEPALIVE
size_t get_selection_start(void) {
    if (!g_has_selection) return g_cursor.position;
    return g_selection_start < g_cursor.position ? g_selection_start : g_cursor.position;
}

EMSCRIPTEN_KEEPALIVE
size_t get_selection_end(void) {
    if (!g_has_selection) return g_cursor.position;
    return g_selection_start > g_cursor.position ? g_selection_start : g_cursor.position;
}

// ═══════════════════════════════════════════════════════════════════════════
// SEARCH
// ═══════════════════════════════════════════════════════════════════════════

static char g_search_pattern[256] = {0};

EMSCRIPTEN_KEEPALIVE
void set_search_pattern(const char* pattern) {
    strncpy(g_search_pattern, pattern, sizeof(g_search_pattern) - 1);
    g_search_pattern[sizeof(g_search_pattern) - 1] = '\0';
}

EMSCRIPTEN_KEEPALIVE
int search_next(void) {
    if (!g_buffer || g_search_pattern[0] == '\0') return 0;
    
    long found = buffer_find_next(g_buffer, g_cursor.position + 1, g_search_pattern);
    if (found >= 0) {
        g_cursor.position = (size_t)found;
        update_cursor_line_col();
        g_cursor.preferred_column = g_cursor.column;
        return 1;
    }
    
    // Wrap around
    found = buffer_find_next(g_buffer, 0, g_search_pattern);
    if (found >= 0) {
        g_cursor.position = (size_t)found;
        update_cursor_line_col();
        g_cursor.preferred_column = g_cursor.column;
        return 1;
    }
    
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int search_prev(void) {
    if (!g_buffer || g_search_pattern[0] == '\0') return 0;
    
    long found = buffer_find_prev(g_buffer, g_cursor.position, g_search_pattern);
    if (found >= 0) {
        g_cursor.position = (size_t)found;
        update_cursor_line_col();
        g_cursor.preferred_column = g_cursor.column;
        return 1;
    }
    
    // Wrap around
    found = buffer_find_prev(g_buffer, buffer_length(g_buffer), g_search_pattern);
    if (found >= 0) {
        g_cursor.position = (size_t)found;
        update_cursor_line_col();
        g_cursor.preferred_column = g_cursor.column;
        return 1;
    }
    
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// UTILITY
// ═══════════════════════════════════════════════════════════════════════════

EMSCRIPTEN_KEEPALIVE
void free_string(char* str) {
    free(str);
}

// Main entry point
int main(void) {
    EM_ASM({
        console.log('[mix-core] WASM module loaded');
    });
    return 0;
}
