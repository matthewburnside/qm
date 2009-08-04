#ifndef _QM_H
#define _QM_H

#include <sys/queue.h>
#include "truth.h"

struct term {
     char *v;
     int bits;
     int mark;
     int cover[128];
     int cover_len;
     TAILQ_ENTRY(term) entry;
};

TAILQ_HEAD(term_list, term);

struct expr *qm(struct expr *expr, struct symtab *symtab, int symlen);

#endif
