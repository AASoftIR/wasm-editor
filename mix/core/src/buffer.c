// buffer.c - Piece Table Text Buffer Implementation

#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_ADD_CAPACITY 4096
#define INITIAL_PIECE_CAPACITY 64
#define INITIAL_LINE_CAPACITY 1024

// ═══════════════════════════════════════════════════════════════════════════
// INTERNAL HELPERS
// ═══════════════════════════════════════════════════════════════════════════

static bool ensure_add_capacity(Buffer* buffer, size_t additional) {
    size_t needed = buffer->add_length + additional;
    if (needed <= buffer->add_capacity) return true;
    
    size_t new_capacity = buffer->add_capacity * 2;
    while (new_capacity < needed) new_capacity *= 2;
    
    char* new_buffer = realloc(buffer->add_buffer, new_capacity);
    if (!new_buffer) return false;
    
    buffer->add_buffer = new_buffer;
    buffer->add_capacity = new_capacity;
    return true;
}

static bool ensure_piece_capacity(Buffer* buffer) {
    if (buffer->piece_count < buffer->piece_capacity) return true;
    
    size_t new_capacity = buffer->piece_capacity * 2;
    Piece* new_pieces = realloc(buffer->pieces, new_capacity * sizeof(Piece));
    if (!new_pieces) return false;
    
    buffer->pieces = new_pieces;
    buffer->piece_capacity = new_capacity;
    return true;
}

static void rebuild_line_cache(Buffer* buffer) {
    if (!buffer->lines_dirty) return;
    
    // Get all text
    char* text = buffer_get_all(buffer);
    if (!text) return;
    
    // Count lines
    size_t line_count = 1;
    for (size_t i = 0; i < buffer->total_length; i++) {
        if (text[i] == '\n') line_count++;
    }
    
    // Reallocate if needed
    if (line_count > buffer->line_count || !buffer->line_starts) {
        size_t new_capacity = line_count + INITIAL_LINE_CAPACITY;
        size_t* new_starts = realloc(buffer->line_starts, new_capacity * sizeof(size_t));
        if (!new_starts) {
            free(text);
            return;
        }
        buffer->line_starts = new_starts;
    }
    
    // Build line index
    buffer->line_starts[0] = 0;
    size_t line = 1;
    for (size_t i = 0; i < buffer->total_length; i++) {
        if (text[i] == '\n' && i + 1 <= buffer->total_length) {
            buffer->line_starts[line++] = i + 1;
        }
    }
    
    buffer->line_count = line_count;
    buffer->lines_dirty = false;
    free(text);
}

// Find which piece contains position, returns piece index and offset within piece
static bool find_piece_at(Buffer* buffer, size_t position, size_t* piece_index, size_t* offset) {
    size_t current_pos = 0;
    
    for (size_t i = 0; i < buffer->piece_count; i++) {
        if (position < current_pos + buffer->pieces[i].length) {
            *piece_index = i;
            *offset = position - current_pos;
            return true;
        }
        current_pos += buffer->pieces[i].length;
    }
    
    // Position at end
    if (position == buffer->total_length) {
        *piece_index = buffer->piece_count;
        *offset = 0;
        return true;
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// PUBLIC API
// ═══════════════════════════════════════════════════════════════════════════

Buffer* buffer_create(const char* initial_content) {
    Buffer* buffer = calloc(1, sizeof(Buffer));
    if (!buffer) return NULL;
    
    // Initialize add buffer
    buffer->add_buffer = malloc(INITIAL_ADD_CAPACITY);
    if (!buffer->add_buffer) {
        free(buffer);
        return NULL;
    }
    buffer->add_capacity = INITIAL_ADD_CAPACITY;
    buffer->add_length = 0;
    
    // Initialize pieces
    buffer->pieces = malloc(INITIAL_PIECE_CAPACITY * sizeof(Piece));
    if (!buffer->pieces) {
        free(buffer->add_buffer);
        free(buffer);
        return NULL;
    }
    buffer->piece_capacity = INITIAL_PIECE_CAPACITY;
    buffer->piece_count = 0;
    
    // Initialize original content
    if (initial_content && *initial_content) {
        size_t len = strlen(initial_content);
        buffer->original = malloc(len + 1);
        if (!buffer->original) {
            free(buffer->pieces);
            free(buffer->add_buffer);
            free(buffer);
            return NULL;
        }
        memcpy(buffer->original, initial_content, len + 1);
        buffer->original_length = len;
        
        // Create initial piece
        buffer->pieces[0].source = SOURCE_ORIGINAL;
        buffer->pieces[0].start = 0;
        buffer->pieces[0].length = len;
        buffer->piece_count = 1;
        buffer->total_length = len;
    } else {
        buffer->original = NULL;
        buffer->original_length = 0;
        buffer->total_length = 0;
    }
    
    buffer->lines_dirty = true;
    return buffer;
}

void buffer_destroy(Buffer* buffer) {
    if (!buffer) return;
    free(buffer->original);
    free(buffer->add_buffer);
    free(buffer->pieces);
    free(buffer->line_starts);
    free(buffer);
}

size_t buffer_length(Buffer* buffer) {
    return buffer ? buffer->total_length : 0;
}

size_t buffer_line_count(Buffer* buffer) {
    if (!buffer) return 0;
    rebuild_line_cache(buffer);
    return buffer->line_count;
}

bool buffer_insert(Buffer* buffer, size_t position, const char* text, size_t length) {
    if (!buffer || !text || length == 0) return false;
    if (position > buffer->total_length) return false;
    
    // Append to add buffer
    if (!ensure_add_capacity(buffer, length)) return false;
    size_t add_start = buffer->add_length;
    memcpy(buffer->add_buffer + add_start, text, length);
    buffer->add_length += length;
    
    // Find piece to split
    size_t piece_idx, offset;
    if (!find_piece_at(buffer, position, &piece_idx, &offset)) return false;
    
    // Create new piece for inserted text
    Piece new_piece = {
        .source = SOURCE_ADD,
        .start = add_start,
        .length = length
    };
    
    if (piece_idx == buffer->piece_count) {
        // Insert at end
        if (!ensure_piece_capacity(buffer)) return false;
        buffer->pieces[buffer->piece_count++] = new_piece;
    } else if (offset == 0) {
        // Insert before piece
        if (!ensure_piece_capacity(buffer)) return false;
        memmove(&buffer->pieces[piece_idx + 1], &buffer->pieces[piece_idx],
                (buffer->piece_count - piece_idx) * sizeof(Piece));
        buffer->pieces[piece_idx] = new_piece;
        buffer->piece_count++;
    } else {
        // Split piece
        Piece* old = &buffer->pieces[piece_idx];
        Piece left = { old->source, old->start, offset };
        Piece right = { old->source, old->start + offset, old->length - offset };
        
        // Need space for 2 new pieces (split + insert)
        while (buffer->piece_count + 2 > buffer->piece_capacity) {
            if (!ensure_piece_capacity(buffer)) return false;
        }
        
        // Shift pieces right
        memmove(&buffer->pieces[piece_idx + 3], &buffer->pieces[piece_idx + 1],
                (buffer->piece_count - piece_idx - 1) * sizeof(Piece));
        
        buffer->pieces[piece_idx] = left;
        buffer->pieces[piece_idx + 1] = new_piece;
        buffer->pieces[piece_idx + 2] = right;
        buffer->piece_count += 2;
    }
    
    buffer->total_length += length;
    buffer->lines_dirty = true;
    return true;
}

bool buffer_delete(Buffer* buffer, size_t position, size_t length) {
    if (!buffer || length == 0) return false;
    if (position + length > buffer->total_length) return false;
    
    size_t start_piece, start_offset;
    size_t end_piece, end_offset;
    
    if (!find_piece_at(buffer, position, &start_piece, &start_offset)) return false;
    if (!find_piece_at(buffer, position + length, &end_piece, &end_offset)) return false;
    
    // Simple case: delete within one piece
    if (start_piece == end_piece) {
        Piece* piece = &buffer->pieces[start_piece];
        
        if (start_offset == 0 && end_offset == piece->length) {
            // Delete entire piece
            memmove(&buffer->pieces[start_piece], &buffer->pieces[start_piece + 1],
                    (buffer->piece_count - start_piece - 1) * sizeof(Piece));
            buffer->piece_count--;
        } else if (start_offset == 0) {
            // Delete from start
            piece->start += length;
            piece->length -= length;
        } else if (end_offset == piece->length) {
            // Delete to end
            piece->length = start_offset;
        } else {
            // Split piece
            if (!ensure_piece_capacity(buffer)) return false;
            
            Piece right = {
                piece->source,
                piece->start + end_offset,
                piece->length - end_offset
            };
            piece->length = start_offset;
            
            memmove(&buffer->pieces[start_piece + 2], &buffer->pieces[start_piece + 1],
                    (buffer->piece_count - start_piece - 1) * sizeof(Piece));
            buffer->pieces[start_piece + 1] = right;
            buffer->piece_count++;
        }
    } else {
        // Multi-piece delete
        // Trim first piece
        buffer->pieces[start_piece].length = start_offset;
        
        // Trim last piece
        if (end_piece < buffer->piece_count) {
            buffer->pieces[end_piece].start += end_offset;
            buffer->pieces[end_piece].length -= end_offset;
        }
        
        // Remove pieces in between
        size_t remove_count = end_piece - start_piece - 1;
        if (remove_count > 0) {
            memmove(&buffer->pieces[start_piece + 1], &buffer->pieces[end_piece],
                    (buffer->piece_count - end_piece) * sizeof(Piece));
            buffer->piece_count -= remove_count;
        }
        
        // Remove empty pieces
        for (size_t i = 0; i < buffer->piece_count; ) {
            if (buffer->pieces[i].length == 0) {
                memmove(&buffer->pieces[i], &buffer->pieces[i + 1],
                        (buffer->piece_count - i - 1) * sizeof(Piece));
                buffer->piece_count--;
            } else {
                i++;
            }
        }
    }
    
    buffer->total_length -= length;
    buffer->lines_dirty = true;
    return true;
}

char buffer_char_at(Buffer* buffer, size_t position) {
    if (!buffer || position >= buffer->total_length) return '\0';
    
    size_t piece_idx, offset;
    if (!find_piece_at(buffer, position, &piece_idx, &offset)) return '\0';
    
    Piece* piece = &buffer->pieces[piece_idx];
    const char* source = (piece->source == SOURCE_ORIGINAL) 
        ? buffer->original 
        : buffer->add_buffer;
    
    return source[piece->start + offset];
}

char* buffer_get_text(Buffer* buffer, size_t start, size_t length) {
    if (!buffer || start + length > buffer->total_length) return NULL;
    
    char* result = malloc(length + 1);
    if (!result) return NULL;
    
    size_t written = 0;
    size_t current_pos = 0;
    
    for (size_t i = 0; i < buffer->piece_count && written < length; i++) {
        Piece* piece = &buffer->pieces[i];
        size_t piece_end = current_pos + piece->length;
        
        if (piece_end > start) {
            size_t copy_start = (start > current_pos) ? start - current_pos : 0;
            size_t copy_end = (start + length < piece_end) 
                ? start + length - current_pos 
                : piece->length;
            size_t copy_len = copy_end - copy_start;
            
            const char* source = (piece->source == SOURCE_ORIGINAL)
                ? buffer->original
                : buffer->add_buffer;
            
            memcpy(result + written, source + piece->start + copy_start, copy_len);
            written += copy_len;
        }
        
        current_pos = piece_end;
    }
    
    result[length] = '\0';
    return result;
}

char* buffer_get_all(Buffer* buffer) {
    return buffer_get_text(buffer, 0, buffer->total_length);
}

char* buffer_get_line(Buffer* buffer, size_t line_number) {
    if (!buffer) return NULL;
    rebuild_line_cache(buffer);
    
    if (line_number >= buffer->line_count) return NULL;
    
    size_t start = buffer->line_starts[line_number];
    size_t end;
    
    if (line_number + 1 < buffer->line_count) {
        end = buffer->line_starts[line_number + 1];
    } else {
        end = buffer->total_length;
    }
    
    // Exclude newline from result
    size_t length = end - start;
    if (length > 0) {
        char last = buffer_char_at(buffer, end - 1);
        if (last == '\n') length--;
    }
    
    return buffer_get_text(buffer, start, length);
}

size_t buffer_line_start(Buffer* buffer, size_t line_number) {
    if (!buffer) return 0;
    rebuild_line_cache(buffer);
    
    if (line_number >= buffer->line_count) {
        return buffer->total_length;
    }
    
    return buffer->line_starts[line_number];
}

size_t buffer_line_length(Buffer* buffer, size_t line_number) {
    if (!buffer) return 0;
    rebuild_line_cache(buffer);
    
    if (line_number >= buffer->line_count) return 0;
    
    size_t start = buffer->line_starts[line_number];
    size_t end = (line_number + 1 < buffer->line_count)
        ? buffer->line_starts[line_number + 1]
        : buffer->total_length;
    
    return end - start;
}

void buffer_pos_to_line_col(Buffer* buffer, size_t position, size_t* line, size_t* col) {
    if (!buffer || !line || !col) return;
    rebuild_line_cache(buffer);
    
    *line = 0;
    *col = position;
    
    for (size_t i = 0; i < buffer->line_count; i++) {
        if (i + 1 < buffer->line_count && buffer->line_starts[i + 1] <= position) {
            *line = i + 1;
            *col = position - buffer->line_starts[i + 1];
        } else if (buffer->line_starts[i] <= position) {
            *line = i;
            *col = position - buffer->line_starts[i];
            break;
        }
    }
}

size_t buffer_line_col_to_pos(Buffer* buffer, size_t line, size_t col) {
    if (!buffer) return 0;
    rebuild_line_cache(buffer);
    
    if (line >= buffer->line_count) {
        line = buffer->line_count - 1;
    }
    
    size_t line_start = buffer->line_starts[line];
    size_t line_len = buffer_line_length(buffer, line);
    
    if (col > line_len) col = line_len;
    
    return line_start + col;
}

long buffer_find_next(Buffer* buffer, size_t start, const char* needle) {
    if (!buffer || !needle) return -1;
    
    size_t needle_len = strlen(needle);
    if (needle_len == 0) return -1;
    
    char* text = buffer_get_all(buffer);
    if (!text) return -1;
    
    for (size_t i = start; i + needle_len <= buffer->total_length; i++) {
        if (strncmp(text + i, needle, needle_len) == 0) {
            free(text);
            return (long)i;
        }
    }
    
    free(text);
    return -1;
}

long buffer_find_prev(Buffer* buffer, size_t start, const char* needle) {
    if (!buffer || !needle) return -1;
    
    size_t needle_len = strlen(needle);
    if (needle_len == 0 || start < needle_len) return -1;
    
    char* text = buffer_get_all(buffer);
    if (!text) return -1;
    
    for (size_t i = start - needle_len; ; i--) {
        if (strncmp(text + i, needle, needle_len) == 0) {
            free(text);
            return (long)i;
        }
        if (i == 0) break;
    }
    
    free(text);
    return -1;
}

bool buffer_replace(Buffer* buffer, size_t position, size_t old_length, 
                    const char* new_text, size_t new_length) {
    if (!buffer_delete(buffer, position, old_length)) return false;
    if (new_length > 0) {
        if (!buffer_insert(buffer, position, new_text, new_length)) return false;
    }
    return true;
}
