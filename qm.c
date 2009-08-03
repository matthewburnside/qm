#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>

#include "qm.h"
#include "truth.h"
#include "parse.h"


struct term {
     char *v;
     int len;
     int mark;
     TAILQ_ENTRY(term) entry;
};

TAILQ_HEAD(term_list, term);



struct term *
new_term(int len)
{
     struct term *p = (struct term *)malloc(sizeof(struct term));
     p->v = malloc(len * sizeof(char));
     p->len = len;
     p->mark = 0;
     return p;
}

void
free_term(struct term *term)
{
     free(term->v);
     free(term);
}

/* return the valth minterm of truth */
struct term *
get_term(struct truth *truth, int val)
{
     struct term *p;
     int i;

     p = new_term(truth->vars);
     for (i = 0; i < truth->vars; i++)
	  p->v[i] = tt_bit(val, i);     

     return p;
}

/* returns # of non-matching bits between p and q, or -1 if p and q
 * cannot be compared. */
int
term_cmp(struct term *p, struct term *q)
{
     int i, diff = 0;

     if (p->len != q->len)
	  return -1;
     for (i = 0; i < p->len; i++) {
	  if (p->v[i] == DC && q->v[i] == DC)       /* ignore matching DCs */
	       continue;
	  if (!(p->v[i] == DC) ^ !(q->v[i] == DC))  /* non-match is fail   */
	       return -1;
	  if (p->v[i] != q->v[i])
	       diff++;
     }
     return diff;
}


/* returns a merge of p and q, or NULL if a merge cannot be made */
struct term *
term_merge(struct term *p, struct term *q)
{
     struct term *t;
     int i, m;

     if (term_cmp(p, q) != 1)
	  return NULL;
     
     t = new_term(p->len);

     for (i = 0; i < p->len; i++)
	  if (p->v[i] == q->v[i])
	       t->v[i] = p->v[i];
	  else
	       t->v[i] = DC;

     return t;
}

void
print_term(struct term *term) {
     int i;

     printf("%s ", term->mark == 1 ? "" : "*");
     for (i = 0; i < term->len; i++)
	  printf("%s ", (term->v[i]) == 0 ? "0" :
	       (term->v[i] == 1 ? "1" : "-"));
}

void
print_terms(struct term_list *terms) {
     struct term *term;
     int i;

     TAILQ_FOREACH(term, terms, entry) {
	  print_term(term);
	  printf("\n");
     }
}


int
get_terms(struct truth *truth, struct term_list *terms)
{
     struct term *p;
     int i, j, pn = 0;

     for (i = 0; i < truth->entries; i++)
	  if (truth->tab[i]) {
	       p = get_term(truth, i);
	       TAILQ_INSERT_TAIL(terms, p, entry);
	       pn++;
	  }

     return pn;
}
     

int
reduce(struct term_list *from, struct term_list *to)
{
     struct term *p, *q, *t;
     int len = 0;
     
     for (p = TAILQ_FIRST(from); p; p = TAILQ_NEXT(p, entry)) {	  
	  for (q = TAILQ_NEXT(p, entry); q; q = TAILQ_NEXT(q, entry)) {	       
	       if ((t = term_merge(p, q)) != NULL) {
		    p->mark = q->mark = 1;
		    TAILQ_INSERT_TAIL(to, t, entry);
		    len++;
	       }
	  }
     }
     
     return len;
}



#define PASS_MAX 128
struct truth *
pass1(struct truth *truth)
{
     struct term_list termtab[PASS_MAX];
     struct term *term;
     int i, n;
     int c0, c1;

     TAILQ_INIT(&termtab[0]);
     n = get_terms(truth, &termtab[0]);
     printf("\n%d terms:\n", n);
     print_terms(&termtab[0]);

     for (i = 1; i < 128; i++) {
	  TAILQ_INIT(&termtab[i]);

	  c1 = reduce(&termtab[i-1], &termtab[i]);
	  printf("\n%d reduced terms:\n", c1);
	  print_terms(&termtab[i]);

	  if (c1 == 0)
	       break;

	  c0 = c1;
     }

     printf("\nReduced list:\n");
     for (i = 0; i < 128; i++) {
	  if (TAILQ_FIRST(&termtab[i]) == NULL)
	       break;
	  TAILQ_FOREACH(term, &termtab[i], entry) {
	       if (!term->mark) {
		    print_term(term);
		    printf("\n");
	       }
	  }
     }

     return NULL;

}


/*

(b*-c)+(-a*c*d)+(a*-b*d)

*/

struct expr *
qm(struct expr *expr, struct symtab *symtab, int symlen)
{
     struct truth *truth;
     int i, j;

     truth = truthtab(expr, symtab, symlen);
     print_tt(truth);

     pass1(truth);

     return NULL;

}
