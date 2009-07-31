#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "qm.h"

#define ONE  0x1
#define ZERO 0x0
#define DC   0x2

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
     struct term *p;

     p = (struct term *)malloc(sizeof(struct term));
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
term_match(struct term *p, struct term *q)
{
     int i, diff = 0;

     if (p->len != q->len)
	  return -1;
     for (i = 0; i < p->len; i++) {
	  /* ignore matching DCs */
	  if (p->v[i] == DC && q->v[i] == DC)
	       continue;
	  /* non-match is fail */
	  if (!(p->v[i] == DC) ^ !(q->v[i] == DC))
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

     if (term_match(p, q) != 1)
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
	  printf("%d ", term->v[i]);
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

     for (i = 0; i < truth->len; i++)
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
	  if (p->mark)
	       continue;

	  for (q = TAILQ_NEXT(p, entry); q; q = TAILQ_NEXT(q, entry)) {	       
	       if (q->mark)
		    continue;
	       
	       if ((t = term_merge(p, q)) != NULL) {
		    p->mark = q->mark = 1;
		    TAILQ_INSERT_TAIL(to, t, entry);
		    len++;
	       }
	  }
     }
     
     return len;
}




struct truth *
blah(struct truth *truth)
{
     struct term_list terms[128];
     struct term *term;
     int i;
     int c0, c1;

     TAILQ_INIT(&terms[0]);
     c0 = get_terms(truth, &terms[0]);
     printf("\n%d terms:\n", c0);
     print_terms(&terms[0]);


     for (i = 1; i < 128; i++) {
	  TAILQ_INIT(&terms[i]);

	  c1 = reduce(&terms[i-1], &terms[i]);
	  printf("\n%d reduced terms:\n", c1);
	  print_terms(&terms[i]);

	  if (c0 == c1)
	       break;

	  c0 = c1;
     }

     printf("\nReduced list:\n");
     for (i = 0; i < 128; i++) {
	  if (TAILQ_FIRST(&terms[i]) == NULL)
	       break;
	  TAILQ_FOREACH(term, &terms[i], entry) {
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

     /* generate the truth table */
     truth = truthtab(symlen);
     for (i = 0; i < truth->len; i++)
	  truth->tab[i] = eval(expr, i);

     /* print the truth table */
     for (j = 0; j < symlen; j++)
	  printf("%s\t", symtab[j].name);
     printf("eval\n");
     for (i = 0; i < truth->len; i++) {
	  for (j = 0; j < symlen; j++)
	       printf("%d\t", tt_bit(i, j));
	  printf("%d\n", truth->tab[i]);
     }


     return NULL;
/*      printf("a\n"); fflush(stdout); */

}
