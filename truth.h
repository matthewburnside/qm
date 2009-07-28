#ifndef _TRUTH_H
#define _TRUTH_H

#include "parse.h"

struct truth {
     unsigned char *tab;
     int len;
     int vars;
};

struct truth    *truthtab(int len);
unsigned char   get_val(unsigned int tt_index, unsigned int sym_index);
unsigned char   eval(struct bool *tree, int tt_index);

#endif
