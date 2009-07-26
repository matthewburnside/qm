%{
#include "canonical.tab.h"
%}

%%

[ \t]*[\n]            { /* nothing */ }
^[ \t]*               { /* nothing */ }

[ \t]+AND[ \t]+|[ \t]+and[ \t]+   { return AND; }
[ \t]+OR[ \t]+|[ \t]+or[ \t]+     { return OR; }

-                     { return NOT; }
\([ \t]*              { return LP; }
[ \t]*\)              { return RP; }

[[:alnum:]]+          { sprintf(yylval.s, "%s", yytext); return TERM; }
\"([^"\n]|\\["\n])*\" { sprintf(yylval.s, "%s", yytext); return TERM; }
\'([^'\n]|\\['\n])*\' { sprintf(yylval.s, "%s", yytext); return TERM; }

[ \t]+                { return WS; }

%%
