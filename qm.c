#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qm.h"
#include "truth.h"
#include "parse.h"


struct term {
     char *v;
     int bits;
     int mark;
     int cover[128];
     int cover_len;
     TAILQ_ENTRY(term) entry;
};

TAILQ_HEAD(term_list, term);



struct term *
new_term(int bits)
{
     struct term *p = (struct term *)malloc(sizeof(struct term));
     p->v = malloc(bits * sizeof(char));
     p->bits = bits;
     p->mark = 0;
     p->cover_len = 0;
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
term_match(struct term *p, struct term *q)
{
     int i, diff = 0;

     if (p->bits != q->bits)
	  return -1;
     for (i = 0; i < p->bits; i++) {
	  if (p->v[i] == DC && q->v[i] == DC)       /* ignore matching DCs */
	       continue;
	  if (!(p->v[i] == DC) ^ !(q->v[i] == DC))  /* non-match is fail   */
	       return -1;
	  if (p->v[i] != q->v[i])
	       diff++;
     }
     return diff;
}

/* returns 1 if p and q are equal, 0 otherwise. */
int
term_cmp(struct term *p, struct term *q)
{
     int i;

     if (p->bits != q->bits)
	  return 0;

     for (i = 0; i < p->bits; i++)
	  if (p->v[i] != q->v[i])
	       return 0;

     return 1;
}


/* returns a merge of p and q, or NULL if a merge cannot be made */
struct term *
term_merge(struct term *p, struct term *q)
{
     struct term *t;
     int i, m;

     if (term_match(p, q) != 1)
	  return NULL;
     
     t = new_term(p->bits);

     for (i = 0; i < p->bits; i++)
	  if (p->v[i] == q->v[i])
	       t->v[i] = p->v[i];
	  else
	       t->v[i] = DC;

     for (i = 0; i < p->cover_len; i++)
	  t->cover[t->cover_len++] = p->cover[i];

     for (i = 0; i < q->cover_len; i++)
	  t->cover[t->cover_len++] = q->cover[i];


     return t;
}

void
print_term(struct term *term) {
     int i;

     printf("(");
     for (i = 0; i < term->cover_len; i++) {
	  printf("%d ", term->cover[i]);
     }
     printf(")\t");

     for (i = 0; i < term->bits; i++) {
	  printf("%s ", (term->v[i]) == 0 ? "0" :
	       (term->v[i] == 1 ? "1" : "-"));
     }

     printf("%s ", term->mark == 1 ? "" : "*");

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
minterms(struct truth *truth, struct term_list *minterms)
{
     struct term *p;
     int i, j, pn = 0;

     for (i = 0; i < truth->entries; i++)
	  if (truth->tab[i]) {
	       p = get_term(truth, i);
	       p->cover[p->cover_len++] = i;
	       TAILQ_INSERT_TAIL(minterms, p, entry);
	       pn++;
	  }

     return pn;
}
     

int
reduce(struct term_list *from, struct term_list *to)
{
     struct term *p, *q, *r, *t;
     int len = 0, dup;

     for (p = TAILQ_FIRST(from); p; p = TAILQ_NEXT(p, entry)) {	  
	  for (q = TAILQ_NEXT(p, entry); q; q = TAILQ_NEXT(q, entry)) {	       
	       if ((t = term_merge(p, q)) != NULL) {
		    p->mark = q->mark = 1;
		    dup = 0;

		    TAILQ_FOREACH(r, to, entry) {
			 if (term_cmp(t, r) == 1) {
			      dup = 1;
			      break;
			 }
		    }
		    if (dup)
			 continue;

		    TAILQ_INSERT_TAIL(to, t, entry);
		    len++;

/* 		    t->minterm[t->mn++] = XXX; */

	       }
	  }
     }
     return len;
}



#define PASS_MAX 128

int
prime_implicants(struct term_list *minterms, struct term_list *primes)
{
     struct term_list termtab[PASS_MAX];
     struct term *term, *cp;
     int i, n;
     int c0, c1;

     TAILQ_INIT(&termtab[0]);
     TAILQ_FOREACH(term, minterms, entry) {
	  cp = (struct term *)malloc(sizeof(struct term));
	  memcpy(cp, term, sizeof(struct term));
	  TAILQ_INSERT_TAIL(&termtab[0], cp, entry);
     }

     for (i = 1; i < PASS_MAX; i++) {
	  TAILQ_INIT(&termtab[i]);

	  c1 = reduce(&termtab[i-1], &termtab[i]);

	  if (c1 == 0)
	       break;

	  printf("\n%d reduced terms:\n", c1);
	  print_terms(&termtab[i]);

	  c0 = c1;
     }

/*      printf("\nReduced list:\n"); */
/*      for (i = 0; i < PASS_MAX; i++) { */
/* 	  if (TAILQ_FIRST(&termtab[i]) == NULL) */
/* 	       break; */
/* 	  TAILQ_FOREACH(term, &termtab[i], entry) { */
/* 	       if (!term->mark) { */
/* 		    print_term(term); */
/* 		    printf("\n"); */
/* 	       } */
/* 	  } */
/*      } */


     n = 0;
     for (i = 0; i < PASS_MAX; i++) {
	  if (TAILQ_FIRST(&termtab[i]) == NULL)
	       break;
	  TAILQ_FOREACH(term, &termtab[i], entry) {
	       if (!term->mark) {
		    n++;
		    cp = (struct term *)malloc(sizeof(struct term));
		    memcpy(cp, term, sizeof(struct term));
		    TAILQ_INSERT_TAIL(primes, cp, entry);
	       }
	  }
     }

     
     /* XXX: free termtab */

     return n;
}

void
reduce_primes(struct term_list *minterms,
              int mn,
              struct term_list *primes,
              int pn,
              struct term_list *reduced)
{
     unsigned char **primetab;
     int i;

     primetab = (unsigned char **)calloc(pn, sizeof(unsigned char *));
     for (i = 0; i < pn; i++)
	  primetab[i] = (unsigned char *)calloc(mn, sizeof(unsigned char));

     

     
}



struct expr *
qm(struct expr *expr, struct symtab *symtab, int symlen)
{
     struct truth *truth;
     struct term_list *minterms_l, primes_l, reduced_l;
     int mn, pn, rn;

     truth = truthtab(expr, symtab, symlen);
     print_tt(truth);

     minterms_l = (struct term_list *)malloc(sizeof(struct term_list));

     TAILQ_INIT(minterms_l);
     mn = minterms(truth, minterms_l);
     printf("\n%d minterms:\n", mn);
     print_terms(minterms_l);
     

     TAILQ_INIT(&primes_l);
     pn = prime_implicants(minterms_l, &primes_l);
     printf("\n%d primes:\n", pn);
     print_terms(&primes_l);


/*      TAILQ_INIT(reduced_l); */
/*      rn = reduce_primes(minterms, mn, primes, pn, reduced); */



     return NULL;

}

/*

clear && ./qm "(-a*-b*-c*-d)+(-a*c*d)+(a*b*-c)+(a*b*-d)+(b*c*d)"


*/
