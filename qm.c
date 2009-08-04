#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qm.h"
#include "truth.h"
#include "parse.h"


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


/* returns a merge of p and q, or NULL if a merge cannot be made */
struct term *
term_merge(struct term *p, struct term *q)
{
     struct term *t;
     int i, m, diff = 0;

     /* can a match be made? */
     if (p->bits != q->bits)
	  return NULL;
     for (i = 0; i < p->bits; i++) {
	  if (p->v[i] == DC && q->v[i] == DC) /* ignore matching DCs */
	       continue;
	  if (!(p->v[i] == DC) ^ !(q->v[i] == DC)) /* non-match is fail   */
	       return NULL;
	  if (p->v[i] != q->v[i])
	       diff++;
     }
     if (diff != 1)
	  return NULL;
     
     /* create the new term */
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
	       p = new_term(truth->vars);
	       for (j = 0; j < truth->vars; j++)
		    p->v[j] = tt_bit(i, j);     	       
	       p->cover[p->cover_len++] = i;
	       TAILQ_INSERT_TAIL(minterms, p, entry);
	       pn++;
	  }
     return pn;
}
     

int
list_merge(struct term_list *from, struct term_list *to)
{
     struct term *p, *q, *r, *t;
     int len = 0, dup, i;

     TAILQ_FOREACH(p, from, entry) {
	  for (q = TAILQ_NEXT(p, entry); q; q = TAILQ_NEXT(q, entry)) {	       
	       if ((t = term_merge(p, q)) != NULL) {
		    p->mark = q->mark = 1;

		    /* have we already made a merge like this one? */
		    dup = 0;
		    TAILQ_FOREACH(r, to, entry) {
			 if (t->bits != r->bits)
			      continue;
			 i = 0;
			 while (t->v[i] == r->v[i] && i < t->bits)
			      i++;
			 if (i == t->bits) {
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
	  c1 = list_merge(&termtab[i-1], &termtab[i]);
	  if (c1 == 0)
	       break;
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


int
prime_implicants2(struct term_list *minterms, struct term_list *primes)
{
     
}


int
min_cover(struct term_list *minterms,
     int minterms_len,
     struct term_list *primes,
     int primes_len,
     struct term_list *cover)
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
	  TAILQ_INSERT_HEAD(cover, p, entry);
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
build_expr(struct term_list *list)
{
     struct expr *root, *seedling, *p, *q;
     struct term *term;
     int i;

     root = NULL;
     TAILQ_FOREACH(term, list, entry) {
	  seedling = NULL;
	  for (i = 0; i < term->bits; i++) {
	       switch (term->v[i]) {
	       case ZERO:
		    q = new_expr(VAR);		    
		    q->u.var.sym = i;
		    p = new_expr(NOT_EXPR);
		    p->u.not.b = q;
		    break;
	       case ONE:
		    p = new_expr(VAR);
		    p->u.var.sym = i;
		    break;
	       case DC:
		    continue;
	       }

	       if (seedling) {
		    q = new_expr(AND_EXPR);
		    q->u.and.l = seedling;
		    q->u.and.r = p;
		    seedling = q;
	       } else
		    seedling = p;
	  }

	  q = new_expr(PAREN_EXPR);
	  q->u.paren.b = seedling;
	  seedling = q;

	  printf("\n\n");
	  print_expr(seedling);
	  printf("\n\n");

	  if (root) {
	       p = new_expr(OR_EXPR);
	       p->u.or.l = root;
	       p->u.or.r = q;
	       root = p;
	  } else
	       root = seedling;
     }
     return root;
}









































struct expr *
build_expr2(struct term_list *list)
{
     struct expr *root, *p, *q, *parent;
     struct term *term;
     int i;

     root = NULL;
     TAILQ_FOREACH(term, list, entry) {

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
     struct term_list terms, primes, cover;
     int mn, pn, rn;

     truth = truthtab(expr, symtab, symlen);
     print_tt(truth);

     TAILQ_INIT(&terms);
     mn = minterms(truth, &terms);
     printf("\n%d minterms:\n", mn);
     print_terms(&terms);
     

     TAILQ_INIT(&primes);
     pn = prime_implicants(&terms, &primes);
     printf("\n%d primes:\n", pn);
     print_terms(&primes);


     TAILQ_INIT(&cover);
     rn = min_cover(&terms, mn, &primes, pn, &cover);
     printf("\n%d cover:\n", rn);
     print_terms(&cover);
          
     printf("\n");
     return build_expr(&cover);

}
