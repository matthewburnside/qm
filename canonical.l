%{
#include "canonical.h"
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

[[:alnum:]]+          { yylval.s = yytext; return TERM; }
\"([^"\n]|\\["\n])*\" { yylval.s = yytext; return TERM; }
\'([^'\n]|\\['\n])*\' { yylval.s = yytext; return TERM; }

[ \t]+                { return WS; }

%%
