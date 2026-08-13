#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

SymbolTable* create_symbol_table(void) {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Memory allocation failed for Symbol Table.\n");
        exit(1);
    }
    table->head = NULL;
    table->count = 0;
    return table;
}

void add_symbol(SymbolTable *table, const char *identifier, SymbolCategory category, const char *target, int line, int column) {
    if (!table || !identifier) return;

    SymbolEntry *entry = (SymbolEntry *)malloc(sizeof(SymbolEntry));
    if (!entry) {
        fprintf(stderr, "Memory allocation failed for Symbol Entry.\n");
        exit(1);
    }

    entry->identifier = duplicate_string(identifier);
    entry->category = category;
    entry->target = target ? duplicate_string(target) : duplicate_string("N/A");
    entry->line = line;
    entry->column = column;
    entry->next = table->head;
    table->head = entry;
    table->count++;
}

SymbolEntry* lookup_symbol(SymbolTable *table, const char *identifier, SymbolCategory category) {
    if (!table || !identifier) return NULL;
    SymbolEntry *curr = table->head;
    while (curr) {
        if (curr->category == category && strcmp(curr->identifier, identifier) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void print_symbol_table(const SymbolTable *table) {
    if (!table) return;

    printf("======================================================================\n");
    printf("                             SYMBOL TABLE                             \n");
    printf("======================================================================\n");
    printf("%-5s | %-12s | %-20s | %-20s | %-8s\n", "No.", "Category", "Identifier / Text", "Target / URL", "Location");
    printf("----------------------------------------------------------------------\n");

    if (table->count == 0) {
        printf("  (Symbol Table is Empty)\n");
        printf("======================================================================\n");
        return;
    }

    SymbolEntry *curr = table->head;
    int idx = 1;
    while (curr) {
        const char *cat_str = (curr->category == SYMBOL_HEADING) ? "HEADING" : "LINK";
        char loc_buf[32];
        snprintf(loc_buf, sizeof(loc_buf), "L%d:C%d", curr->line, curr->column);

        char id_buf[21];
        strncpy(id_buf, curr->identifier ? curr->identifier : "", 20);
        id_buf[20] = '\0';

        char target_buf[21];
        strncpy(target_buf, curr->target ? curr->target : "", 20);
        target_buf[20] = '\0';

        printf("%-5d | %-12s | %-20s | %-20s | %-8s\n",
               idx++, cat_str, id_buf, target_buf, loc_buf);
        curr = curr->next;
    }
    printf("======================================================================\n");
}

void free_symbol_table(SymbolTable *table) {
    if (!table) return;
    SymbolEntry *curr = table->head;
    while (curr) {
        SymbolEntry *next = curr->next;
        if (curr->identifier) free(curr->identifier);
        if (curr->target) free(curr->target);
        free(curr);
        curr = next;
    }
    free(table);
}

static void extract_ast_text(ASTNode *node, char *buf, size_t max_len) {
    if (!node) return;
    if (node->text) {
        size_t cur_len = strlen(buf);
        if (cur_len < max_len - 1) {
            strncat(buf, node->text, max_len - cur_len - 1);
        }
    }
    for (int i = 0; i < node->child_count; i++) {
        extract_ast_text(node->children[i], buf, max_len);
    }
}

static void sanitize_slug(const char *src, char *slug, size_t max_len) {
    size_t out_idx = 0;
    for (size_t i = 0; src[i] != '\0' && out_idx < max_len - 1; i++) {
        if (isalnum((unsigned char)src[i])) {
            slug[out_idx++] = tolower((unsigned char)src[i]);
        } else if (src[i] == ' ' || src[i] == '-') {
            if (out_idx > 0 && slug[out_idx - 1] != '-') {
                slug[out_idx++] = '-';
            }
        }
    }
    if (out_idx > 0 && slug[out_idx - 1] == '-') out_idx--;
    slug[out_idx] = '\0';
}

void populate_symbol_table(ASTNode *ast, SymbolTable *table) {
    if (!ast || !table) return;

    if (ast->type == NODE_HEADER) {
        char text_buf[256] = "";
        extract_ast_text(ast, text_buf, sizeof(text_buf));
        char slug_buf[256] = "";
        sanitize_slug(text_buf, slug_buf, sizeof(slug_buf));

        if (strlen(text_buf) > 0) {
            add_symbol(table, text_buf, SYMBOL_HEADING, slug_buf, ast->line_number, ast->column_number);
        }
    } else if (ast->type == NODE_LINK) {
        char text_buf[256] = "";
        extract_ast_text(ast, text_buf, sizeof(text_buf));
        const char *url = ast->url ? ast->url : "N/A";
        add_symbol(table, text_buf[0] != '\0' ? text_buf : "[No Display Text]",
                   SYMBOL_LINK, url, ast->line_number, ast->column_number);
    }

    for (int i = 0; i < ast->child_count; i++) {
        populate_symbol_table(ast->children[i], table);
    }
}
