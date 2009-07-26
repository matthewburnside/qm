%{
#include "canonical.h"
#include "canonical.tab.h"

%}

%%

[ \t]*[\n]                      { /* empty */ }
^[ \t]*                         { /* empty */ }

[ \t]+AND[ \t]+|[ \t]+and[ \t]+ { return AND; }
[ \t]+OR[ \t]+|[ \t]+or[ \t]+   { return OR; }

-                               { return NOT; }
\([ \t]*                        { return LP; }
[ \t]*\)                        { return RP; }

[[:alnum:]]+                    { yylval.s = sym(yytext); return TERM; }
\"([^"\n]|\\["\n])*\"           { yylval.s = sym(yytext); return TERM; }
\'([^'\n]|\\['\n])*\'           { yylval.s = sym(yytext); return TERM; }

[ \t]+                          { return WS; }

%%
