#include <stdio.h>
#include <stdlib.h>

#include "truth.h"

struct truth *
truthtab(struct expr *expr, struct symtab *symtab, int symlen)
{
	struct truth *t = (struct truth *)malloc(sizeof(struct truth));
	int i;

	t->symtab = symtab;
	t->vars = symlen;
	t->entries = 1 << symlen;
	t->tab = (unsigned char *)malloc((t->entries) * sizeof(unsigned char));

	for (i = 0; i < t->entries; i++)
	     t->tab[i] = eval(expr, i);

	return t;
}

void
print_tt(struct truth *tt)
{
     int i, j;

     for (j = 0; j < tt->vars; j++)
	  printf("%s\t", tt->symtab[j].name);
     printf("f()\n");
     for (i = 0; i < tt->entries; i++) {
	  for (j = 0; j < tt->vars; j++)
	       printf("%d\t", tt_bit(i, j));
	  printf("%d\n", tt->tab[i]);
     }
}

unsigned char
tt_bit(unsigned int entry, unsigned int bit)
{
     return (entry & (1 << bit) ? 1 : 0);
}

unsigned char
eval(struct expr *expr, int tt_e)
{
	switch (expr->type) {
	case VAR:
	    return tt_bit(tt_e, expr->u.var.sym);
	case OR_EXPR:
	    return eval(expr->u.or.l, tt_e) | eval(expr->u.or.r, tt_e);
	case AND_EXPR:
	    return eval(expr->u.and.l, tt_e) & eval(expr->u.and.r, tt_e);
	case NOT_EXPR:
	    return eval(expr->u.not.b, tt_e) ^ 1;
	case PAREN_EXPR:
	    return eval(expr->u.paren.b, tt_e);
	default:
	    yyerror("unknown type");
	    exit(1);
	}
}

