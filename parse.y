%{
#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "qm.h"

//int yydebug = 1;

int     yyerror(char *str);
int     yyparse(void);

struct expr     *expr;
struct symtab   symtab[NSYMS];
int             symlen = 0;
%}

%token	<s> TERM
%token  AND OR
%token  LP RP
%token  NOT
%token  WS

%type   <b> query expr

%left       OR
%left       AND WS
%nonassoc   NOT

%union {
    int             s;
    struct expr *   b;
}

%%

query
: { /* nothing */ }
| expr {
    expr = $1;
}
;

expr
: TERM {
    $$ = new_expr(VAR);
    $$->u.var.sym = $1;
}
| expr AND expr {
    $$ = new_expr(AND_EXPR);
    $$->u.and.l = $1;
    $$->u.and.r = $3;
}
| expr OR expr {
    $$ = new_expr(OR_EXPR);
    $$->u.or.l = $1;
    $$->u.or.r = $3;
}
| NOT expr {
    $$ = new_expr(NOT_EXPR);
    $$->u.not.b = $2;
}
| LP expr RP {
    $$ = new_expr(PAREN_EXPR);
    $$->u.paren.b = $2;
}
;

%%

/* return the index for s in the symbol table */
int
symbol(char *s)
{
    int i;

    for (i = 0; i < symlen; i++)
        if (!strcmp(s, symtab[i].name))
            return i;

    if (symlen >= NSYMS) {
        yyerror("too many symbols");
        exit(1);
    }

    symtab[symlen++].name = strdup(s);
    return symlen - 1;
}


struct expr *
new_expr(enum expr_type type)
{
    struct expr *expr = (struct expr *)malloc(sizeof(struct expr));
    expr->type = type;
    return expr;
}


void
print_expr(struct expr *t)
{
    if (t == NULL) {
        yyerror("empty expresion");
        exit(1);
    }

    switch (t->type) {
    case VAR:
        printf("%s", symtab[t->u.var.sym]);
        break;
    case OR_EXPR:
        print_expr(t->u.or.l); printf("+"); print_expr(t->u.or.r);
        break;
    case AND_EXPR:
        print_expr(t->u.and.l); printf("*"); print_expr(t->u.and.r);
        break;
    case NOT_EXPR:
        printf("-"); print_expr(t->u.not.b);
        break;
    case PAREN_EXPR:
        printf("("); print_expr(t->u.paren.b); printf(")");
        break;
    default:
        yyerror("unknown type");
        exit(1);
    }
}


int
yyerror(char *str)
{
    printf("%s\n", str);
    exit(1);
}


int
main(int argc, char *argv[])
{
    struct expr *reduce;

    if (argc > 1)
        yy_scan_string(argv[1]);

    yyparse();

    reduce = qm(expr, symtab, symlen);
    print_expr(reduce);
    printf("\n");

    return 0;
}
