#include "html_generator.h"
#include <stdio.h>
#include <stdlib.h>

int count_html_elements(ASTNode *node) {
    if (!node) return 0;
    int count = 0;
    if (node->type != NODE_DOCUMENT && node->type != NODE_TEXT) {
        count = 1;
    }
    for (int i = 0; i < node->child_count; i++) {
        count += count_html_elements(node->children[i]);
    }
    return count;
}

static void generate_node(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case NODE_DOCUMENT:
            fprintf(out, "<!DOCTYPE html>\n<html>\n<head>\n<title>Converted Markdown</title>\n");
            fprintf(out, "<style>\n");
            fprintf(out, "body { font-family: 'Segoe UI', system-ui, -apple-system, sans-serif; line-height: 1.7; max-width: 800px; margin: 40px auto; padding: 0 20px; color: #2d3748; background-color: #f7fafc; }\n");
            fprintf(out, "h1, h2, h3 { color: #1a202c; margin-top: 1.2em; border-bottom: 2px solid #e2e8f0; padding-bottom: 0.3em; }\n");
            fprintf(out, "h1 { color: #2b6cb0; }\n");
            fprintf(out, "a { color: #3182ce; text-decoration: none; font-weight: 500; }\n");
            fprintf(out, "a:hover { text-decoration: underline; }\n");
            fprintf(out, "ul, ol { background: #ffffff; padding: 15px 30px; border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.1); }\n");
            fprintf(out, "li { margin-bottom: 6px; }\n");
            fprintf(out, "strong { color: #2c5282; }\n");
            fprintf(out, "em { color: #742a2a; }\n");
            fprintf(out, "</style>\n</head>\n<body>\n");
            for (int i = 0; i < node->child_count; i++) generate_node(node->children[i], out);
            fprintf(out, "</body>\n</html>\n");
            break;
        case NODE_HEADER:
            fprintf(out, "<h%d>", node->level);
            for (int i = 0; i < node->child_count; i++) generate_node(node->children[i], out);
            fprintf(out, "</h%d>\n", node->level);
            break;
        case NODE_PARAGRAPH:
            fprintf(out, "<p>");
            for (int i = 0; i < node->child_count; i++) generate_node(node->children[i], out);
            fprintf(out, "</p>\n");
            break;
        case NODE_BOLD:
            fprintf(out, "<strong>");
            for (int i = 0; i < node->child_count; i++) generate_node(node->children[i], out);
            fprintf(out, "</strong>");
            break;
        case NODE_ITALIC:
            fprintf(out, "<em>");
            for (int i = 0; i < node->child_count; i++) generate_node(node->children[i], out);
            fprintf(out, "</em>");
            break;
        case NODE_TEXT:
            if (node->text) fprintf(out, "%s", node->text);
            break;
        case NODE_UNORDERED_LIST:
            fprintf(out, "<ul>\n");
            for (int i = 0; i < node->child_count; i++) generate_node(node->children[i], out);
            fprintf(out, "</ul>\n");
            break;
        case NODE_ORDERED_LIST:
            fprintf(out, "<ol>\n");
            for (int i = 0; i < node->child_count; i++) generate_node(node->children[i], out);
            fprintf(out, "</ol>\n");
            break;
        case NODE_LIST_ITEM:
            fprintf(out, "  <li>");
            for (int i = 0; i < node->child_count; i++) generate_node(node->children[i], out);
            fprintf(out, "</li>\n");
            break;
        case NODE_LINK:
            fprintf(out, "<a href=\"%s\">", node->url ? node->url : "");
            for (int i = 0; i < node->child_count; i++) generate_node(node->children[i], out);
            fprintf(out, "</a>");
            break;
    }
}

void generate_html(ASTNode *ast, const char *output_file) {
    FILE *out = fopen(output_file, "w");
    if (!out) {
        perror("Error opening output file");
        return;
    }
    generate_node(ast, out);
    fclose(out);
}
