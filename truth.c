#include <stdio.h>
#include <stdlib.h>

#include "truth.h"

struct truth *
truthtab(int len)
{
	struct truth *t;

	t = malloc(sizeof(struct truth));
	t->len = 1 << len;
	t->tab = malloc((1 << len) * sizeof(char));

	return t;
}

unsigned int
get_val(unsigned int tt_index, unsigned int sym_index)
{
	return (tt_index & (1 << sym_index) ? 1 : 0);
}

unsigned int
eval(struct bool *t, int tt_index)
{
	switch (t->type) {
	case VAR:
	    return get_val(tt_index, XXX);
	case OR_EXPR:
	    return eval(t->u.or.l) | eval(t->u.or.r);
	case AND_EXPR:
	    return eval(t->u.and.l) & eval(t->u.and.r);
	case NOT_EXPR:
	    return eval(t->u.not.b) ^ 1;
	case PAREN_EXPR:
	    return eval(t->u.paren.b);
	default:
	    yyerror("unknown type");
	    exit(1);
	}
}

