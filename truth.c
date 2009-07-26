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

int
get_val(unsigned int tt_index, unsigned int sym_index)
{
	return (tt_index & (1 << sym_index) ? 1 : 0);
}

