#ifndef _TRUTH_H
#define _TRUTH_H

#include "parse.h"

struct truth {
     unsigned char *tab;
     int len;
     int vars;
};

/* allocation a truth table with vars variables */
struct truth    *truthtab(int vars);

/* return bit bit from truth table entry */
unsigned char   tt_bit(unsigned int entry, unsigned int bit);

/* evaluate expr using values from truth table entry tt_e */
unsigned char   eval(struct expr *tree, int tt_index);

#endif
