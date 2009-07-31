%{
#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "truth.h"
#include "qm.h"

//int yydebug = 1;

     int	        yyerror(char *str);
     int	        yyparse(void);
     int	        yylex(void);

     struct bool     *tree;
     struct symtab   symtab[NSYMS];
     int             symsiz = 0;

     %}

%token	<s> TERM
%token  AND OR
%token  LP RP
%token  NOT
%token  WS

%type   <b> query expr

%left   OR
%left   AND WS
%nonassoc   NOT


%union {
     int s;
     struct bool *b;
}

%%

query   : { /* nothing */ }
| expr {
     tree = $1;
}
;

expr
: TERM {
     $$ = (struct bool *)malloc(sizeof(struct bool));
     $$->type = VAR;
     $$->u.var.sym = $1;
}
| expr AND expr {
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

  /* returns an index into the symbol table */
int
symbol(char *s)
{
     int i;

     for (i = 0; i < symsiz; i++)
	  if (!strcmp(s, symtab[i].name))
	       return i;

     if (symsiz >= NSYMS) {
	  yyerror("too many symbols");
	  exit(1);
     }

     symtab[symsiz++].name = strdup(s);
     return symsiz-1;
}

void
print_tree(struct bool *t)
{
     if (!t) {
	  yyerror("empty expresion");
	  exit(1);
     }

     switch (t->type) {
     case VAR:
	  printf("%s", symtab[t->u.var.sym]);
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


/*

(b*-c)+(-a*c*d)+(a*-b*d)

*/

int
main(int argc, char *argv[])
{
     struct truth    *truth, *reduce;
     int i, j;

     if (argc > 1) {
	  printf("scanning: %s\n", argv[1]);
	  yy_scan_string(argv[1]);
     }

     yyparse();

     print_tree(tree);
     printf("\n");

     /* generate the truth table */
     truth = truthtab(symsiz);
     for (i = 0; i < truth->len; i++)
	  truth->tab[i] = eval(tree, i);

     /* print the truth table */
     for (j = 0; j < symsiz; j++)
	  printf("%s\t", symtab[j].name);
     printf("eval\n");
     for (i = 0; i < truth->len; i++) {
	  for (j = 0; j < symsiz; j++)
	       printf("%d\t", tt_bit(i, j));
	  printf("%d\n", truth->tab[i]);
     }

/*      reduce = qm(truth); */

     return 0;
}

