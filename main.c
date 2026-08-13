#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "html_generator.h"
#include "utils.h"

static int count_total_lines(const char *buffer) {
    int lines = 1;
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == '\n') lines++;
    }
    return lines;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.md> <output.html>\n", argv[0]);
        return 1;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    FILE *f = fopen(input_filename, "rb");
    if (!f) {
        perror("Error opening input file");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(f);
        return 1;
    }
    
    size_t read_bytes = fread(buffer, 1, length, f);
    buffer[read_bytes] = '\0';
    fclose(f);

    int total_lines = count_total_lines(buffer);

    printf("====================================\n");
    printf("Markdown-to-HTML Compiler\n");
    printf("====================================\n\n");

    printf("Input File:\n%s\n\n", input_filename);

    // Phase 1: Lexical Analysis
    printf("Phase 1 : Lexical Analysis\n");
    TokenList tokens = lexer_tokenize(buffer);
    printf("- Total Tokens: %d\n", tokens.count);
    printf("- Token Table:\n");
    printf("----------------------------------------------------------------------\n");
    printf("%-6s | %-18s | %s\n", "Line", "Token Type", "Lexeme Value");
    printf("----------------------------------------------------------------------\n");
    for (int i = 0; i < tokens.count; i++) {
        printf("%-6d | %-18s | %s\n", 
               tokens.items[i].line_number, 
               token_type_to_string(tokens.items[i].type), 
               tokens.items[i].value);
    }
    printf("----------------------------------------------------------------------\n\n");

    // Phase 2: Syntax Analysis
    printf("Phase 2 : Syntax Analysis\n");
    printf("- Parsing Started\n");
    ParserStatus p_status;
    ASTNode *ast = parse_document(&tokens, &p_status);
    if (p_status.error_count == 0) {
        printf("- Parsing Successful\n");
        printf("- Abstract Syntax Tree (AST):\n");
        printf("----------------------------------------------------------------------\n");
        print_ast(ast, 0);
        printf("----------------------------------------------------------------------\n\n");
    } else {
        printf("- Parsing Finished with %d syntax error(s)\n\n", p_status.error_count);
    }

    // Phase 3: Semantic Analysis
    printf("Phase 3 : Semantic Analysis\n");
    printf("- Validating markdown...\n");
    int sem_errors = semantic_analyze(ast);
    if (sem_errors == 0) {
        printf("- Semantic Analysis Passed\n\n");
    } else {
        printf("- Semantic Analysis Failed with %d error(s)\n\n", sem_errors);
    }

    // Phase 4: HTML Code Generation
    printf("Phase 4 : HTML Code Generation\n");
    printf("- Generating %s...\n", output_filename);
    generate_html(ast, output_filename);
    int total_html_elements = count_html_elements(ast);
    printf("- Total HTML Elements Generated: %d\n\n", total_html_elements);

    // Summary
    printf("====================================\n");
    printf("Compilation Successful.\n");
    printf("====================================\n\n");

    printf("Input File:\n%s\n\n", input_filename);
    printf("Output File:\n%s\n\n", output_filename);

    printf("Total Tokens: %d\n", tokens.count);
    printf("Total Lines: %d\n", total_lines);
    printf("Total HTML Elements: %d\n", total_html_elements);

    free_ast(ast);
    free_token_list(&tokens);
    free(buffer);

    return 0;
}
