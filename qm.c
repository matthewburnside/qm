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


/* returns a merge of p and q, or NULL if a merge cannot be made */
struct term *
term_merge(struct term *p, struct term *q)
{
     struct term *t;
     int i, m, diff = 0;

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


/* merge terms in from and store them in to.  return lenth of to.  */
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


int
prime_implicants(struct term_list *minterms, struct term_list *primes)
{
     struct tl_entry {
	  struct term_list tl;
	  TAILQ_ENTRY(tl_entry) entry;
     } *tp, *tq;
     TAILQ_HEAD(term_table, tl_entry) termtab;
     struct term *term, *cp;
     int i = 0, n;

     TAILQ_INIT(&termtab);

     /* minterm list is first column of the prime table */
     tp = (struct tl_entry *)malloc(sizeof(struct tl_entry));
     TAILQ_INIT(&tp->tl);
     tp->tl.tqh_first = minterms->tqh_first;
     tp->tl.tqh_last = minterms->tqh_last;

     /* list_merge() each column to create next column.  */
     tq = tp;
     do {
	  tp = tq;
	  tq = (struct tl_entry *)malloc(sizeof(struct tl_entry));
	  TAILQ_INIT(&tq->tl);
	  TAILQ_INSERT_TAIL(&termtab, tq, entry);
     } while (list_merge(&tp->tl, &tq->tl) != 0);

     n = 0;
     TAILQ_FOREACH(tp, &termtab, entry) {
	  TAILQ_FOREACH(term, &tp->tl, entry) {
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
min_cover(struct term_list *primes, int primes_len, struct term_list *cover)
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


void
print_term(struct term *term) {
     int i;
     printf("( ");
     for (i = 0; i < term->cover_len; i++) {
	  printf("%d ", term->cover[i]);
     }
     printf(")\t\t");
     for (i = 0; i < term->bits; i++) {
	  printf("%s ", (term->v[i]) == 0 ? "0" :
	       (term->v[i] == 1 ? "1" : "-"));
     }
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


struct expr *
qm(struct expr *expr, struct symtab *symtab, int symlen)
{
     struct truth *truth;
     struct term_list terms, primes, cover;
     int mn, pn, rn;

     TAILQ_INIT(&terms);
     TAILQ_INIT(&primes);
     TAILQ_INIT(&cover);

     truth = truthtab(expr, symtab, symlen);

     mn = minterms(truth, &terms);
     pn = prime_implicants(&terms, &primes);
     rn = min_cover(&primes, pn, &cover);
     
     return build_expr(&cover);
}


