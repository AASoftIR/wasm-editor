// buffer.h - Text Buffer Interface
// Implements a piece table data structure for efficient text editing

#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdbool.h>

// ═══════════════════════════════════════════════════════════════════════════
// PIECE TABLE STRUCTURE
// ═══════════════════════════════════════════════════════════════════════════
//
// A piece table consists of:
// 1. Original buffer - the initial text (never modified)
// 2. Add buffer - all inserted text (append-only)
// 3. Pieces - a list of (source, start, length) tuples
//
// Example:
//   Original: "Hello World"
//   After inserting "Beautiful " at position 6:
//   Add buffer: "Beautiful "
//   Pieces: [(ORIG, 0, 6), (ADD, 0, 10), (ORIG, 6, 5)]
//   Result: "Hello Beautiful World"

typedef enum {
    SOURCE_ORIGINAL,
    SOURCE_ADD
} PieceSource;

typedef struct {
    PieceSource source;
    size_t start;
    size_t length;
} Piece;

typedef struct {
    // Original text buffer (read-only)
    char* original;
    size_t original_length;
    
    // Add buffer (append-only)
    char* add_buffer;
    size_t add_length;
    size_t add_capacity;
    
    // Piece table
    Piece* pieces;
    size_t piece_count;
    size_t piece_capacity;
    
    // Cached total length
    size_t total_length;
    
    // Line cache for fast line operations
    size_t* line_starts;
    size_t line_count;
    bool lines_dirty;
} Buffer;

// ═══════════════════════════════════════════════════════════════════════════
// BUFFER OPERATIONS
// ═══════════════════════════════════════════════════════════════════════════

// Create a new buffer with optional initial content
Buffer* buffer_create(const char* initial_content);

// Free buffer memory
void buffer_destroy(Buffer* buffer);

// Get total text length
size_t buffer_length(Buffer* buffer);

// Get line count
size_t buffer_line_count(Buffer* buffer);

// Insert text at position
bool buffer_insert(Buffer* buffer, size_t position, const char* text, size_t length);

// Delete text from position
bool buffer_delete(Buffer* buffer, size_t position, size_t length);

// Get character at position
char buffer_char_at(Buffer* buffer, size_t position);

// Get a range of text (caller must free result)
char* buffer_get_text(Buffer* buffer, size_t start, size_t length);

// Get all text (caller must free result)
char* buffer_get_all(Buffer* buffer);

// Get line content (caller must free result)
char* buffer_get_line(Buffer* buffer, size_t line_number);

// Get line start position
size_t buffer_line_start(Buffer* buffer, size_t line_number);

// Get line length (including newline if present)
size_t buffer_line_length(Buffer* buffer, size_t line_number);

// Convert position to line/column
void buffer_pos_to_line_col(Buffer* buffer, size_t position, size_t* line, size_t* col);

// Convert line/column to position
size_t buffer_line_col_to_pos(Buffer* buffer, size_t line, size_t col);

// ═══════════════════════════════════════════════════════════════════════════
// SEARCH OPERATIONS
// ═══════════════════════════════════════════════════════════════════════════

// Find next occurrence of text starting from position
// Returns -1 if not found
long buffer_find_next(Buffer* buffer, size_t start, const char* needle);

// Find previous occurrence of text
long buffer_find_prev(Buffer* buffer, size_t start, const char* needle);

// Replace text at position
bool buffer_replace(Buffer* buffer, size_t position, size_t old_length, const char* new_text, size_t new_length);

#endif // BUFFER_H
