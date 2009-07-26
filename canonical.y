%{
#include <string.h>
#include <stdlib.h>

#include "parse.h"
/* #include "truth.h" */

//int yydebug = 1;

int	        yyerror(char *str);
int	        yyparse(void);
int	        yylex(void);

struct bool     *tree;
struct symtab   symtab[NSYMS];
int             simsiz = 0;

struct truth    *truth;

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
	struct symtab *s;
	struct bool *b;
}

%%

query   : { /* nothing */ }
        | expr {
		tree = $1;
        }
;

expr    : TERM {
		$$ = (struct bool *)malloc(sizeof(struct bool));
		$$->type = VAR;
		$$->u.var.sym = $1
        }
        | expr AND expr {
		$$ = (struct bool *)malloc(sizeof(struct bool));
		$$->type = AND_EXPR;
		$$->u.and.l = $1;
		$$->u.and.r = $3;
        }
        | expr WS expr {
		$$ = (struct bool *)malloc(sizeof(struct bool));
		$$->type = AND_EXPR;
		$$->u.and.l = $1;
		$$->u.and.r = $3;
        }
        | expr OR expr {
		$$ = (struct bool *)malloc(sizeof(struct bool));
		$$->type = OR_EXPR;
		$$->u.or.l = $1;
		$$->u.or.r = $3;
        }
        | NOT expr {
		$$ = (struct bool *)malloc(sizeof(struct bool));
		$$->type = NOT_EXPR;
		$$->u.not.b = $2;
        }
        | LP expr RP {
		$$ = (struct bool *)malloc(sizeof(struct bool));
		$$->type = PAREN_EXPR;
		$$->u.paren.b = $2;
        }
;

%%

struct symtab *
symbol(char *s)
{
	struct symtab *sp;

	for (sp = symtab; sp < &symtab[NSYMS]; sp++) {
	    if (sp->name && !strcmp(sp->name, s))
		return sp;

	    if (!sp->name) {
		sp->name = strdup(s);
		simsiz++;
		return sp;
	    }
	}
	yyerror("too many symbols");
	exit(1);
}

void
print_tree(struct bool *t)
{
	switch (t->type) {
	case VAR:
	    printf("%s", t->u.var.sym->name);
	    break;
	case OR_EXPR:
	    print_tree(t->u.or.l); printf("+"); print_tree(t->u.or.r);
	    break;
	case AND_EXPR:
	    print_tree(t->u.and.l); printf("*"); print_tree(t->u.and.r);
	    break;
	case NOT_EXPR:
	    printf("-"); print_tree(t->u.not.b);
	    break;
	case PAREN_EXPR:
	    printf("("); print_tree(t->u.paren.b); printf(")");
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
}

int
main(int argc, char *argv)
{
	int i;

	yyparse();

	print_tree(tree);
	printf("\n");

/* 	truth = truthtab(symsiz); */

/* 	for (i = 0; i < truth->len; i++) { */
/* 	    truth->tab[i] = evaluate(tree, assign(symtab, simsiz, truth, i)); */
/* 	} */

	return 0;
}

