#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_HEADER1,
    TOKEN_HEADER2,
    TOKEN_HEADER3,
    TOKEN_BOLD,
    TOKEN_ITALIC,
    TOKEN_UNORDERED_LIST,
    TOKEN_ORDERED_LIST,
    TOKEN_LINK_START,
    TOKEN_LINK_MID,
    TOKEN_LINK_END,
    TOKEN_TEXT,
    TOKEN_NEWLINE,
    TOKEN_BLANK_LINE,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line_number;
} Token;

typedef struct {
    Token *items;
    int count;
    int capacity;
} TokenList;

TokenList lexer_tokenize(const char *source);
void free_token_list(TokenList *list);
const char* token_type_to_string(TokenType type);

#endif // LEXER_H
