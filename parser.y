%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex(void);
extern int yylineno;
extern FILE *yyin;
extern char *yytext;
%}

%union {
    char *str;
}

%token TOKEN_HEADER1 TOKEN_HEADER2 TOKEN_HEADER3
%token TOKEN_BOLD_START TOKEN_BOLD_END TOKEN_ITALIC_START TOKEN_ITALIC_END
%token TOKEN_UNORDERED_LIST TOKEN_ORDERED_LIST
%token TOKEN_LINK_START TOKEN_LINK_MID TOKEN_LINK_END
%token TOKEN_TEXT TOKEN_NEWLINE TOKEN_BLANK_LINE

%type <str> TOKEN_TEXT inline inline_list

%%

document:
    statements
    ;

statements:
    statements statement
    | statement
    ;

statement:
    TOKEN_HEADER1 inline_list line_end { printf("<h1>%s</h1>\n", $2); free($2); }
    | TOKEN_HEADER2 inline_list line_end { printf("<h2>%s</h2>\n", $2); free($2); }
    | TOKEN_HEADER3 inline_list line_end { printf("<h3>%s</h3>\n", $2); free($2); }
    | TOKEN_UNORDERED_LIST inline_list line_end { printf("<ul><li>%s</li></ul>\n", $2); free($2); }
    | TOKEN_ORDERED_LIST inline_list line_end { printf("<ol><li>%s</li></ol>\n", $2); free($2); }
    | inline_list line_end { printf("<p>%s</p>\n", $1); free($1); }
    | TOKEN_NEWLINE
    | TOKEN_BLANK_LINE
    ;

line_end:
    TOKEN_NEWLINE
    | TOKEN_BLANK_LINE
    ;

inline_list:
    inline_list inline {
        char *buf = (char*)malloc(strlen($1) + strlen($2) + 1);
        sprintf(buf, "%s%s", $1, $2);
        free($1); free($2);
        $$ = buf;
    }
    | inline { $$ = $1; }
    ;

inline:
    TOKEN_TEXT { $$ = $1; }
    | TOKEN_BOLD_START inline_list TOKEN_BOLD_END {
        char *buf = (char*)malloc(strlen($2) + 30);
        sprintf(buf, "<strong>%s</strong>", $2);
        free($2);
        $$ = buf;
    }
    | TOKEN_ITALIC_START inline_list TOKEN_ITALIC_END {
        char *buf = (char*)malloc(strlen($2) + 20);
        sprintf(buf, "<em>%s</em>", $2);
        free($2);
        $$ = buf;
    }
    | TOKEN_LINK_START inline_list TOKEN_LINK_MID TOKEN_TEXT TOKEN_LINK_END {
        char *buf = (char*)malloc(strlen($2) + strlen($4) + 35);
        sprintf(buf, "<a href=\"%s\">%s</a>", $4, $2);
        free($2); free($4);
        $$ = buf;
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Bison Syntax Error [Line %d, Token '%s']: %s\n", yylineno, yytext ? yytext : "EOF", s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
            return 1;
        }
        yyin = file;
    }
    printf("--- Flex & Bison HTML Output Start ---\n");
    yyparse();
    printf("--- Flex & Bison HTML Output End ---\n");
    return 0;
}
