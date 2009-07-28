#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>

#include "qm.h"

struct term {
     char *v;
     int len;
     TAILQ_ENTRY(term) entry;
};

TAILQ_HEAD(term_list, term);


struct term_list *
primes(struct truth *truth)
{
     struct term_list *primes;
     struct term *p;
     int i, j, tn;

     primes = (struct term_list *)malloc(sizeof(struct term_list));
     TAILQ_INIT(primes);

     for (i = 0; i < truth->len; i++) {
	  if (truth->tab[i]) {
	       printf(".\n");
	       p = (struct term *)malloc(sizeof(struct term));
	       p->v = malloc(truth->vars * sizeof(char));
	       p->len = truth->vars;

	       for (j = 0; j < truth->vars; j++)
		    p->v[j] = get_val(i, j);

	       TAILQ_INSERT_TAIL(primes, p, entry);
	  }
     }

     return primes;
}
     

void
print_terms(struct term_list *list) {
     struct term *p;
     int i;

     TAILQ_FOREACH(p, list, entry) {
	  for (i = 0; i < p->len; i++)
	       printf("%d ", p->v[i]);

	  printf("\n");
     }
}

struct truth *
qm(struct truth *truth)
{
     struct term_list *p;

     p = primes(truth);

     printf("\nprimes:\n");
     print_terms(p);
     return NULL;
}
