#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

typedef enum {
    NODE_DOCUMENT,
    NODE_HEADER,
    NODE_PARAGRAPH,
    NODE_BOLD,
    NODE_ITALIC,
    NODE_TEXT,
    NODE_UNORDERED_LIST,
    NODE_ORDERED_LIST,
    NODE_LIST_ITEM,
    NODE_LINK
} NodeType;

typedef struct ASTNode {
    NodeType type;
    int level;          // For headers (1-6)
    char *text;         // For text nodes
    char *url;          // For link nodes
    struct ASTNode **children;
    int child_count;
    int child_capacity;
    int line_number;    // For semantic errors
} ASTNode;

ASTNode* create_node(NodeType type, int line_number);
void add_child(ASTNode *parent, ASTNode *child);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int depth);
char* duplicate_string(const char *src);

#endif // UTILS_H
