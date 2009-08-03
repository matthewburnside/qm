#ifndef _TRUTH_H
#define _TRUTH_H

#include "parse.h"

#define ONE  1
#define ZERO 0
#define DC   2

struct truth {
     struct symtab *symtab;
     int vars;
     int entries;
     unsigned char *tab;
};

/* allocate a truth table with symlen variables */
struct truth    *truthtab(struct expr *expr,
                          struct symtab *symtab,
                          int symlen);

/* return bit bit from truth table entry */
unsigned char   tt_bit(unsigned int entry, unsigned int bit);
void            print_tt(struct truth *tt);

/* evaluate expr using values from truth table entry tt_e */
unsigned char   eval(struct expr *tree, int tt_index);

#endif
