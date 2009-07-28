#include <stdio.h>
#include <stdlib.h>

#include "truth.h"

struct truth *
truthtab(int len)
{
	struct truth *t;

	t = malloc(sizeof(struct truth));
	t->len = 1 << len;
	t->tab = malloc((1 << len) * sizeof(unsigned char));

	return t;
}

unsigned char
get_val(unsigned int tt_index, unsigned int sym_index)
{
	return (tt_index & (1 << sym_index) ? 1 : 0);
}

unsigned char
eval(struct bool *t, int tt_i)
{
	switch (t->type) {
	case VAR:
	    return get_val(tt_i, t->u.var.sym);
	case OR_EXPR:
	    return eval(t->u.or.l, tt_i) | eval(t->u.or.r, tt_i);
	case AND_EXPR:
	    return eval(t->u.and.l, tt_i) & eval(t->u.and.r, tt_i);
	case NOT_EXPR:
	    return eval(t->u.not.b, tt_i) ^ 1;
	case PAREN_EXPR:
	    return eval(t->u.paren.b, tt_i);
	default:
	    yyerror("unknown type");
	    exit(1);
	}
}

