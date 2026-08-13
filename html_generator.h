#ifndef HTML_GENERATOR_H
#define HTML_GENERATOR_H

#include "utils.h"

int count_html_elements(ASTNode *ast);
void generate_html(ASTNode *ast, const char *output_file);

#endif // HTML_GENERATOR_H
