#include <stdio.h>
#include <stdlib.h>

#include "truth.h"

struct truth *
truthtab(int vars)
{
	struct truth *t;

	t = malloc(sizeof(struct truth));
	t->len = 1 << vars;
	t->vars = vars;
	t->tab = malloc((t->len) * sizeof(unsigned char));

	return t;
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

