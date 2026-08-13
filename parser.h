#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "utils.h"

typedef struct {
    int error_count;
} ParserStatus;

ASTNode* parse_document(TokenList *tokens, ParserStatus *status);

#endif // PARSER_H
