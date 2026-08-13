#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ASTNode* create_node(NodeType type, int line_number) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for ASTNode.\n");
        exit(1);
    }
    node->type = type;
    node->level = 0;
    node->text = NULL;
    node->url = NULL;
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    node->line_number = line_number;
    return node;
}

void add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    
    if (parent->child_count == parent->child_capacity) {
        parent->child_capacity = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        parent->children = (ASTNode **)realloc(parent->children, parent->child_capacity * sizeof(ASTNode *));
        if (!parent->children) {
            fprintf(stderr, "Memory allocation failed for AST children.\n");
            exit(1);
        }
    }
    parent->children[parent->child_count++] = child;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }
    if (node->children) free(node->children);
    if (node->text) free(node->text);
    if (node->url) free(node->url);
    free(node);
}

void print_ast(ASTNode *node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) {
        printf("  │ ");
    }
    
    const char *type_names[] = {
        "DOCUMENT", "HEADER", "PARAGRAPH", "BOLD", "ITALIC",
        "TEXT", "UNORDERED_LIST", "ORDERED_LIST", "LIST_ITEM", "LINK"
    };
    
    printf("├── [%s]", type_names[node->type]);
    if (node->level > 0) printf(" (Level %d)", node->level);
    if (node->text) printf(" : \"%s\"", node->text);
    if (node->url) printf(" [URL: %s]", node->url);
    printf("\n");
    
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], depth + 1);
    }
}

char* duplicate_string(const char *src) {
    if (!src) return NULL;
    char *dst = (char *)malloc(strlen(src) + 1);
    if (dst) {
        strcpy(dst, src);
    }
    return dst;
}
