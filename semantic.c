#include "semantic.h"
#include <stdio.h>
#include <string.h>

static int error_count = 0;

static void analyze_node(ASTNode *node) {
    if (!node) return;

    if (node->type == NODE_HEADER && node->child_count == 0) {
        fprintf(stderr, "Semantic Warning [Line %d]: Empty header text.\n", node->line_number);
    }
    
    if (node->type == NODE_BOLD && node->child_count == 0) {
        fprintf(stderr, "Semantic Warning [Line %d]: Empty bold block (**).\n", node->line_number);
    }

    if (node->type == NODE_ITALIC && node->child_count == 0) {
        fprintf(stderr, "Semantic Warning [Line %d]: Empty italic block (*).\n", node->line_number);
    }

    if (node->type == NODE_LINK) {
        if (node->child_count == 0) {
            fprintf(stderr, "Semantic Warning [Line %d]: Link is missing display text.\n", node->line_number);
        }
        if (!node->url || strlen(node->url) == 0) {
            fprintf(stderr, "Semantic Error [Line %d]: Link is missing target URL.\n", node->line_number);
            error_count++;
        }
    }

    for (int i = 0; i < node->child_count; i++) {
        analyze_node(node->children[i]);
    }
}

int semantic_analyze(ASTNode *ast) {
    error_count = 0;
    analyze_node(ast);
    return error_count;
}
