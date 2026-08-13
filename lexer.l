%{
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>

extern int yylineno;
int in_bold = 0;
int in_italic = 0;
%}

%option noyywrap
%option yylineno

%%

"### "         { return TOKEN_HEADER3; }
"## "          { return TOKEN_HEADER2; }
"# "           { return TOKEN_HEADER1; }
"\*\*"          { if (in_bold) { in_bold = 0; return TOKEN_BOLD_END; } else { in_bold = 1; return TOKEN_BOLD_START; } }
"\*"            { if (in_italic) { in_italic = 0; return TOKEN_ITALIC_END; } else { in_italic = 1; return TOKEN_ITALIC_START; } }
"- "           { return TOKEN_UNORDERED_LIST; }
[0-9]+\.\      { return TOKEN_ORDERED_LIST; }
"\["            { return TOKEN_LINK_START; }
"\]\("          { return TOKEN_LINK_MID; }
"\)"            { return TOKEN_LINK_END; }
\r?\n\r?\n      { in_bold = 0; in_italic = 0; return TOKEN_BLANK_LINE; }
\r?\n           { in_bold = 0; in_italic = 0; return TOKEN_NEWLINE; }
[0-9]+          { yylval.str = strdup(yytext); return TOKEN_TEXT; }
[^\r\n\*#\[\]\(\)\-0-9]+ { yylval.str = strdup(yytext); return TOKEN_TEXT; }
.               { yylval.str = strdup(yytext); return TOKEN_TEXT; }

%%
