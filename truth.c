#include <stdio.h>
#include <stdlib.h>

#include "truth.h"

struct truth *
truthtab(int len)
{
	struct truth *t;

	t = malloc(sizeof(struct truth));
	t->len = len;
	t->tab = malloc((1 << len) * sizeof(char));

	return t;
}
