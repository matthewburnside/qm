%{
#include <string.h>
#include <unistd.h>

#include "canonical.h"

//int yydebug = 1;

int	        yyerror(char *str);
int	        yyparse(void);
int	        yylex(void);
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
		$$ = (struct bool *)malloc(sizeof(struct bool));
		$$->type = VAR;
		$$->u.var.name = strdup($1);
		$$->u.var.val = 0;
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

void
print_tree(struct bool *t)
{
	switch (t->type) {
	case VAR:
	    printf("%s", t->u.var.name);
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
	yyparse();
	return 0;
}

