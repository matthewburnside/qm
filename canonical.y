%{
#include <string.h>
#include <unistd.h>

#include "canonical.h"

//int yydebug = 1;

static int errors = 0;

int	        yyerror(char *str);
int	        yyparse(void);
int	        yylex(void);
void            err(int e, const char *msg);
struct bool *   var_expr(char *s);
struct bool *   and_expr(struct bool *l, struct bool *r);
struct bool *   or_expr(struct bool *l, struct bool *r);
struct bool *   not_expr(struct bool *b);
struct bool *   paren_expr(struct bool *b);
void            print_tree(struct bool *t);



%}

%token	<s> TERM
%token  AND OR
%token  LP RP
%token  NOT
%token  WS

%type   <b> query expr

%right  NOT
%left   OR
%left   AND WS

%union {
	char *s;
	struct bool *b;
}

%%

query   : { /* nothing */ }
        | expr {

		print_tree($1);
		printf("\n");

        }
;

expr    : TERM {
	        $$ = var_expr($1);
        }
        | expr AND expr {
	        $$ = and_expr($1, $3);
        }
        | expr WS expr {
	        $$ = and_expr($1, $3);
        }
        | expr OR expr {
	        $$ = or_expr($1, $3);
        }
        | NOT expr {
	        $$ = not_expr($2);
        }
        | LP expr RP {
		$$ = paren_expr($2);
        }
;

%%

struct bool *
var_expr(char *s)
{
	struct bool *p = (struct bool *)malloc(sizeof(struct bool));

	p->type = VAR;
	p->u.var.name = strdup(s);
	p->u.var.val = 0;

	return p;
}

struct bool *
and_expr(struct bool *l, struct bool *r)
{
	struct bool *p = (struct bool *)malloc(sizeof(struct bool));
	p->type = AND_EXPR;
	p->u.and.l = l;
	p->u.and.r = r;
	return p;
}


struct bool *
or_expr(struct bool *l, struct bool *r)
{
	struct bool *p = (struct bool *)malloc(sizeof(struct bool));
	p->type = OR_EXPR;
	p->u.or.l = l;
	p->u.or.r = r;
	return p;
}

struct bool *
not_expr(struct bool *b)
{
	struct bool *p = (struct bool *)malloc(sizeof(struct bool));
	p->type = NOT_EXPR;
	p->u.not.b = b;
	return p;
}

struct bool *
paren_expr(struct bool *b)
{
	struct bool *p = (struct bool *)malloc(sizeof(struct bool));
	p->type = PAREN_EXPR;
	p->u.paren.b = b;
	return p;
}

void
print_tree(struct bool *t)
{
	switch (t->type) {
	case VAR:
	    printf("%s", t->u.var.name);
	    break;
	case OR_EXPR:
	    print_tree(t->u.or.l);
	    printf("+");
	    print_tree(t->u.or.r);
	    break;
	case AND_EXPR:
	    print_tree(t->u.and.l);
	    printf("*");
	    print_tree(t->u.and.r);
	    break;
	case NOT_EXPR:
	    printf("-");
	    print_tree(t->u.not.b);
	    break;
	case PAREN_EXPR:
	    printf("(");
	    print_tree(t->u.not.b);
	    printf(")");
	    break;
	}
}

void
err(int e, const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	_exit(e);
}

int
yyerror(char *str)
{
	errors++;
	printf("%s\n", str);
}

int
main(int argc, char *argv)
{
	yyparse();

	return (errors ? -1 : 0);
}

