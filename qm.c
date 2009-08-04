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
     printf(")\t\t");

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
find_essentials(struct term_list *minterms,
     int minterms_len,
     struct term_list *primes,
     int primes_len)
{

     struct term *term;
     unsigned char **primetab;
     int i, j;

     printf("b: %d, %d\n", primes_len, minterms_len); fflush(stdout);

     primetab = (unsigned char **)calloc(primes_len, sizeof(unsigned char *));
     for (i = 0; i < primes_len; i++)
	  primetab[i] = (unsigned char *)calloc(minterms_len,
	       sizeof(unsigned char));

     printf("c\n\n"); fflush(stdout);

     i = 0;
     TAILQ_FOREACH(term, primes, entry) {
	  i++;

	  print_term(term);
	  printf("  ");
	  printf("%d", term->cover_len);
	  printf("\n");

	 
/* 	  for (j = 0; j < term->cover_len; j++) { */
/* 	       printf("  %d %d\n", i, term->cover[j]); fflush(stdout); */
/* 	       primetab[i][term->cover[j]] = 1; */

/* 	  } */

     }

     printf("x\n"); fflush(stdout);

     for (i = 0; i < primes_len; i++) {
	  for (j = 0; j < minterms_len; j++)
	       printf("%s\t", primetab[i][j] == 1 ? "X" : "");
	  printf("\n");
     }

     printf("y\n"); fflush(stdout);
}

int
reduce_primes(struct term_list *minterms,
              int minterms_len,
              struct term_list *primes,
              int primes_len,
              struct term_list *reduced)
{
     struct term *term, *p, *q;
     int i, j, k, n = primes_len, ret = 0;

     while (n > 0) {
	  p = TAILQ_FIRST(primes);
	  TAILQ_FOREACH(term, primes, entry) {
	       if (term->cover_len > p->cover_len)
		    p = term;
	  }

	  TAILQ_REMOVE(primes, p, entry);
	  TAILQ_INSERT_HEAD(reduced, p, entry);
	  ret++;
	  n--;

	  for (term = TAILQ_FIRST(primes); term != NULL; term = q) {  
	       q = TAILQ_NEXT(term, entry);

	       for (i = 0; i < term->cover_len; i++) {
		    for (j = 0; j < p->cover_len; j++) {
			 if (term->cover[i] == p->cover[j]) {
			      for (k = i; k < term->cover_len; k++)
				   term->cover[k] = term->cover[k+1];
			      term->cover_len--;

			      if (term->cover_len == 0) {
				   TAILQ_REMOVE(primes, term, entry);
				   n--;
			      }

			 }
		    }
	       }
	  }
     }

     return ret;
}


struct expr *
build_expr(struct term_list *reduced)
{
     struct expr *root, *p, *q, *parent;
     struct term *term;
     int i;

     root = NULL;
     TAILQ_FOREACH(term, reduced, entry) {

	  parent = NULL;
	  for (i = 0; i < term->bits; i++) {
	       if (term->v[i] == DC)
		    continue;

	       p = (struct expr *)malloc(sizeof(struct expr));
	       p->type = VAR;
	       p->u.var.sym = i;
	       
	       if (term->v[i] == ZERO) {
		    q = p;
		    p = (struct expr *)malloc(sizeof(struct expr));
		    p->type = NOT_EXPR;
		    p->u.not.b = q;
	       }

	       if (parent == NULL)
		    parent = p;
	       else {
		    q = (struct expr *)malloc(sizeof(struct expr));
		    q->type = AND_EXPR;
		    q->u.and.l = parent;
		    q->u.and.r = p;
		    parent = q;
	       }
	  }

	  p = (struct expr *)malloc(sizeof(struct expr));
	  p->type = PAREN_EXPR;
	  p->u.paren.b = parent;
	  parent = p;

	  if (root == NULL)
	       root = parent;
	  else {
	       q = (struct expr *)malloc(sizeof(struct expr));
	       q->type = OR_EXPR;
	       q->u.or.l = root;
	       q->u.or.r = p;
	       root = q;
	  }
     }

     return root;
}

struct expr *
qm(struct expr *expr, struct symtab *symtab, int symlen)
{
     struct truth *truth;
     struct term_list minterms_l, primes_l, reduced_l;
     int mn, pn, rn;

     truth = truthtab(expr, symtab, symlen);
     print_tt(truth);

     TAILQ_INIT(&minterms_l);
     mn = minterms(truth, &minterms_l);
     printf("\n%d minterms:\n", mn);
     print_terms(&minterms_l);
     

     TAILQ_INIT(&primes_l);
     pn = prime_implicants(&minterms_l, &primes_l);
     printf("\n%d primes:\n", pn);
     print_terms(&primes_l);


     TAILQ_INIT(&reduced_l);
     rn = reduce_primes(&minterms_l, mn, &primes_l, pn, &reduced_l);
     printf("\n%d reduced:\n", rn);
     print_terms(&reduced_l);
          
     printf("\n");
     return build_expr(&reduced_l);

}

/*

clear && ./qm "(-a*-b*-c*-d)+(-a*c*d)+(a*b*-c)+(a*b*-d)+(b*c*d)"

clear && ./qm "(-d*-c*-b*-a)+(-d*-c*b*-a)+(d*-c*-b*-a)+(-d*c*-b*a)+(-d*c*b*-a)+(d*-c*b*-a)+(d*c*-b*-a)+(-d*c*b*a)+(d*c*-b*a)+(d*c*b*-a)+(d*c*b*a)"



 0 -d*-c*-b*-a
 2 -d*-c* b*-a
 8  d*-c*-b*-a
 5 -d* c*-b* a
 6 -d* c* b*-a
10  d*-c* b*-a
12  d* c*-b*-a
 7 -d* c* b* a
13  d* c*-b* a
14  d* c* b*-a
15  d* c* b* a

 


*/
