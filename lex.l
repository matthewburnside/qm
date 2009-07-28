%{
#include "parse.h"
#include "minbool.tab.h"
%}

%option noyywrap

%%
\"([^"\n]|\\["\n])*\"           { yylval.s = symbol(yytext); return TERM; }
\'([^'\n]|\\['\n])*\'           { yylval.s = symbol(yytext); return TERM; }
[[:alnum:]]+                    { yylval.s = symbol(yytext); return TERM; }

[ \n\t]+                        { /* empty */ }

\*                              { return AND; }
\+                              { return OR; }

-                               { return NOT; }
\(                              { return LP; }
\)                              { return RP; }
%%
