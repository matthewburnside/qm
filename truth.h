#ifndef _CANONICAL_H
#define _CANONICAL_H

struct truth {
	char *tab;
	int len;
};

struct truth *truthtab(int len);
int get_val(unsigned int tt_index, unsigned int sym_index);

#endif
