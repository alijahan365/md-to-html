#include "lexer.h"
#include "utils.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

static const char *src = NULL;
static int pos = 0;
static int current_line = 1;
static int is_line_start = 1;

static char peek() { return src[pos]; }
static void advance() { pos++; }
static int is_at_end() { return src[pos] == '\0'; }

static const char* default_lexeme(TokenType type) {
    switch (type) {
        case TOKEN_HEADER1: return "#";
        case TOKEN_HEADER2: return "##";
        case TOKEN_HEADER3: return "###";
        case TOKEN_BOLD: return "**";
        case TOKEN_ITALIC: return "*";
        case TOKEN_UNORDERED_LIST: return "-";
        case TOKEN_ORDERED_LIST: return "1.";
        case TOKEN_LINK_START: return "[";
        case TOKEN_LINK_MID: return "](";
        case TOKEN_LINK_END: return ")";
        case TOKEN_NEWLINE: return "\\n";
        case TOKEN_BLANK_LINE: return "\\n\\n";
        case TOKEN_EOF: return "<EOF>";
        default: return "";
    }
}

static Token make_token(TokenType type, const char *val) {
    Token t;
    t.type = type;
    if (val) {
        t.value = duplicate_string(val);
    } else {
        t.value = duplicate_string(default_lexeme(type));
    }
    t.line_number = current_line;
    return t;
}

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_HEADER1: return "HEADER1";
        case TOKEN_HEADER2: return "HEADER2";
        case TOKEN_HEADER3: return "HEADER3";
        case TOKEN_BOLD: return "BOLD";
        case TOKEN_ITALIC: return "ITALIC";
        case TOKEN_UNORDERED_LIST: return "UNORDERED_LIST";
        case TOKEN_ORDERED_LIST: return "ORDERED_LIST";
        case TOKEN_LINK_START: return "LINK_START";
        case TOKEN_LINK_MID: return "LINK_MID";
        case TOKEN_LINK_END: return "LINK_END";
        case TOKEN_TEXT: return "TEXT";
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_BLANK_LINE: return "BLANK_LINE";
        case TOKEN_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

static void add_token(TokenList *list, Token token) {
    if (list->count == list->capacity) {
        list->capacity = (list->capacity == 0) ? 16 : list->capacity * 2;
        list->items = (Token *)realloc(list->items, list->capacity * sizeof(Token));
    }
    list->items[list->count++] = token;
}

static Token lexer_next_token() {
    if (is_at_end()) return make_token(TOKEN_EOF, NULL);

    char c = peek();

    if (c == '\r') {
        advance();
        return lexer_next_token();
    }

    if (c == '\n') {
        advance();
        if (peek() == '\r') advance();
        if (peek() == '\n') {
            advance();
            Token t = make_token(TOKEN_BLANK_LINE, NULL);
            current_line += 2;
            is_line_start = 1;
            return t;
        }
        Token t = make_token(TOKEN_NEWLINE, NULL);
        current_line++;
        is_line_start = 1;
        return t;
    }

    if (is_line_start) {
        // Headers
        if (c == '#') {
            int hash_count = 0;
            int temp_pos = pos;
            while (src[temp_pos] == '#') {
                hash_count++;
                temp_pos++;
            }
            if (src[temp_pos] == ' ') {
                pos = temp_pos + 1;
                is_line_start = 0;
                if (hash_count == 1) return make_token(TOKEN_HEADER1, NULL);
                if (hash_count == 2) return make_token(TOKEN_HEADER2, NULL);
                if (hash_count >= 3) return make_token(TOKEN_HEADER3, NULL);
            }
        }
        
        // Unordered lists
        if ((c == '-' || c == '*') && src[pos+1] == ' ') {
            pos += 2;
            is_line_start = 0;
            return make_token(TOKEN_UNORDERED_LIST, NULL);
        }

        // Ordered lists
        if (isdigit(c)) {
            int temp = pos;
            while (isdigit(src[temp])) temp++;
            if (src[temp] == '.' && src[temp+1] == ' ') {
                pos = temp + 2;
                is_line_start = 0;
                return make_token(TOKEN_ORDERED_LIST, NULL);
            }
        }
    }

    is_line_start = 0;

    // Bold and Italic
    if (c == '*') {
        if (src[pos+1] == '*') {
            pos += 2;
            return make_token(TOKEN_BOLD, NULL);
        }
        pos++;
        return make_token(TOKEN_ITALIC, NULL);
    }

    // Links
    if (c == '[') { pos++; return make_token(TOKEN_LINK_START, NULL); }
    if (c == ']' && src[pos+1] == '(') { pos += 2; return make_token(TOKEN_LINK_MID, NULL); }
    if (c == ')') { pos++; return make_token(TOKEN_LINK_END, NULL); }

    // Text collection
    int start = pos;
    while (!is_at_end()) {
        char next = peek();
        if (next == '\n' || next == '\r' || next == '*' || next == '[' || (next == ']' && src[pos+1] == '(') || next == ')') {
            break;
        }
        advance();
    }
    
    int len = pos - start;
    if (len == 0) {
        advance();
        return make_token(TOKEN_TEXT, " ");
    }
    
    char *text_val = (char *)malloc(len + 1);
    strncpy(text_val, src + start, len);
    text_val[len] = '\0';
    
    Token t = make_token(TOKEN_TEXT, text_val);
    free(text_val);
    return t;
}

TokenList lexer_tokenize(const char *source) {
    src = source;
    pos = 0;
    current_line = 1;
    is_line_start = 1;

    TokenList list;
    list.items = NULL;
    list.count = 0;
    list.capacity = 0;

    while (1) {
        Token token = lexer_next_token();
        add_token(&list, token);
        if (token.type == TOKEN_EOF) break;
    }

    return list;
}

void free_token_list(TokenList *list) {
    if (!list || !list->items) return;
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].value) {
            free(list->items[i].value);
        }
    }
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}
