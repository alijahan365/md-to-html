#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "utils.h"

typedef enum {
    SYMBOL_HEADING,
    SYMBOL_LINK
} SymbolCategory;

typedef struct SymbolEntry {
    char *identifier;       // Heading text or link display text
    SymbolCategory category;
    char *target;           // URL for links, generated anchor ID for headings
    int line;
    int column;
    struct SymbolEntry *next;
} SymbolEntry;

typedef struct {
    SymbolEntry *head;
    int count;
} SymbolTable;

SymbolTable* create_symbol_table(void);
void add_symbol(SymbolTable *table, const char *identifier, SymbolCategory category, const char *target, int line, int column);
SymbolEntry* lookup_symbol(SymbolTable *table, const char *identifier, SymbolCategory category);
void print_symbol_table(const SymbolTable *table);
void free_symbol_table(SymbolTable *table);
void populate_symbol_table(ASTNode *ast, SymbolTable *table);

#endif // SYMBOL_TABLE_H
