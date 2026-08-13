#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static TokenList *token_list = NULL;
static int token_index = 0;
static ParserStatus *p_status = NULL;

static Token cur_token() {
    if (token_list && token_index < token_list->count) {
        return token_list->items[token_index];
    }
    Token eof_tok;
    eof_tok.type = TOKEN_EOF;
    eof_tok.value = "<EOF>";
    eof_tok.line_number = (token_list && token_list->count > 0) ? token_list->items[token_list->count - 1].line_number : 1;
    return eof_tok;
}

static void advance_token() {
    if (token_list && token_index < token_list->count) {
        token_index++;
    }
}

static void report_syntax_error(const char *expected, Token found) {
    fprintf(stderr, "Syntax Error [Line %d]: Invalid format - Expected '%s', Found '%s' (\"%s\")\n",
            found.line_number, expected, token_type_to_string(found.type), found.value ? found.value : "");
    if (p_status) p_status->error_count++;
}

static ASTNode* parse_inline() {
    Token tok = cur_token();

    if (tok.type == TOKEN_TEXT) {
        printf("  [Parser Step] Consuming TEXT token: \"%s\" (Line %d)\n", tok.value ? tok.value : "", tok.line_number);
        ASTNode *node = create_node(NODE_TEXT, tok.line_number);
        node->text = duplicate_string(tok.value);
        advance_token();
        return node;
    }

    if (tok.type == TOKEN_BOLD) {
        printf("  [Parser Step] Starting BOLD block (Line %d)\n", tok.line_number);
        ASTNode *node = create_node(NODE_BOLD, tok.line_number);
        advance_token();
        while (cur_token().type != TOKEN_BOLD && cur_token().type != TOKEN_EOF && cur_token().type != TOKEN_NEWLINE) {
            add_child(node, parse_inline());
        }
        if (cur_token().type == TOKEN_BOLD) {
            printf("  [Parser Step] Closing BOLD block (Line %d)\n", cur_token().line_number);
            advance_token();
        } else {
            report_syntax_error("** (Closing BOLD delimiter)", cur_token());
        }
        return node;
    }

    if (tok.type == TOKEN_ITALIC) {
        printf("  [Parser Step] Starting ITALIC block (Line %d)\n", tok.line_number);
        ASTNode *node = create_node(NODE_ITALIC, tok.line_number);
        advance_token();
        while (cur_token().type != TOKEN_ITALIC && cur_token().type != TOKEN_EOF && cur_token().type != TOKEN_NEWLINE) {
            add_child(node, parse_inline());
        }
        if (cur_token().type == TOKEN_ITALIC) {
            printf("  [Parser Step] Closing ITALIC block (Line %d)\n", cur_token().line_number);
            advance_token();
        } else {
            report_syntax_error("* (Closing ITALIC delimiter)", cur_token());
        }
        return node;
    }

    if (tok.type == TOKEN_LINK_START) {
        printf("  [Parser Step] Starting LINK element (Line %d)\n", tok.line_number);
        ASTNode *node = create_node(NODE_LINK, tok.line_number);
        advance_token();
        while (cur_token().type != TOKEN_LINK_MID && cur_token().type != TOKEN_EOF && cur_token().type != TOKEN_NEWLINE) {
            add_child(node, parse_inline());
        }
        if (cur_token().type == TOKEN_LINK_MID) {
            advance_token();
            if (cur_token().type == TOKEN_TEXT) {
                if (node->url) free(node->url);
                node->url = duplicate_string(cur_token().value);
                printf("  [Parser Step] Captured LINK URL: \"%s\"\n", node->url);
                advance_token();
            }
            if (cur_token().type == TOKEN_LINK_END) {
                printf("  [Parser Step] Closing LINK element (Line %d)\n", cur_token().line_number);
                advance_token();
            } else {
                report_syntax_error(") (Closing Link URL delimiter)", cur_token());
            }
        } else {
            report_syntax_error("]( (Link Mid delimiter)", cur_token());
        }
        return node;
    }

    if (tok.type == TOKEN_NEWLINE) {
        ASTNode *node = create_node(NODE_TEXT, tok.line_number);
        node->text = duplicate_string(" ");
        advance_token();
        return node;
    }

    ASTNode *node = create_node(NODE_TEXT, tok.line_number);
    node->text = duplicate_string(tok.value ? tok.value : " ");
    advance_token();
    return node;
}

static ASTNode* parse_block() {
    Token tok = cur_token();

    if (tok.type == TOKEN_HEADER1 || tok.type == TOKEN_HEADER2 || tok.type == TOKEN_HEADER3) {
        int level = (tok.type == TOKEN_HEADER1) ? 1 : (tok.type == TOKEN_HEADER2) ? 2 : 3;
        printf("  [Parser Step] Parsing HEADER (Level %d) on Line %d\n", level, tok.line_number);
        ASTNode *node = create_node(NODE_HEADER, tok.line_number);
        node->level = level;

        advance_token();
        while (cur_token().type != TOKEN_NEWLINE && cur_token().type != TOKEN_EOF) {
            add_child(node, parse_inline());
        }
        if (cur_token().type == TOKEN_NEWLINE) advance_token();
        return node;
    }

    if (tok.type == TOKEN_UNORDERED_LIST || tok.type == TOKEN_ORDERED_LIST) {
        NodeType list_type = (tok.type == TOKEN_UNORDERED_LIST) ? NODE_UNORDERED_LIST : NODE_ORDERED_LIST;
        printf("  [Parser Step] Parsing LIST (%s) on Line %d\n", 
               (list_type == NODE_UNORDERED_LIST) ? "Unordered" : "Ordered", tok.line_number);
        ASTNode *list_node = create_node(list_type, tok.line_number);

        while (cur_token().type == TOKEN_UNORDERED_LIST || cur_token().type == TOKEN_ORDERED_LIST) {
            printf("    [Parser Step] Parsing LIST_ITEM on Line %d\n", cur_token().line_number);
            ASTNode *item_node = create_node(NODE_LIST_ITEM, cur_token().line_number);
            advance_token();
            while (cur_token().type != TOKEN_NEWLINE && cur_token().type != TOKEN_EOF) {
                add_child(item_node, parse_inline());
            }
            if (cur_token().type == TOKEN_NEWLINE) advance_token();
            add_child(list_node, item_node);
        }
        return list_node;
    }

    printf("  [Parser Step] Parsing PARAGRAPH on Line %d\n", tok.line_number);
    ASTNode *node = create_node(NODE_PARAGRAPH, tok.line_number);
    while (cur_token().type != TOKEN_NEWLINE && cur_token().type != TOKEN_BLANK_LINE && cur_token().type != TOKEN_EOF) {
        add_child(node, parse_inline());
    }
    if (cur_token().type == TOKEN_NEWLINE || cur_token().type == TOKEN_BLANK_LINE) advance_token();
    return node;
}

ASTNode* parse_document(TokenList *tokens, ParserStatus *status) {
    token_list = tokens;
    token_index = 0;
    p_status = status;
    if (p_status) p_status->error_count = 0;

    printf("- Parsing Steps Log:\n");
    ASTNode *doc = create_node(NODE_DOCUMENT, 1);

    while (cur_token().type != TOKEN_EOF) {
        if (cur_token().type == TOKEN_BLANK_LINE || cur_token().type == TOKEN_NEWLINE) {
            advance_token();
            continue;
        }
        int start_index = token_index;
        add_child(doc, parse_block());

        // Infinite loop prevention for malformed syntax
        if (token_index == start_index) {
            report_syntax_error("Valid Markdown Block", cur_token());
            advance_token();
        }
    }

    return doc;
}

