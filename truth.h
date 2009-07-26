#ifndef _CANONICAL_H
#define _CANONICAL_H

#include "parse.h"

struct truth {
	char *tab;
	int len;
};

struct truth *truthtab(int len);
unsigned int get_val(unsigned int tt_index, unsigned int sym_index);
unsigned int eval(struct bool *tree, int tt_index);

#endif
