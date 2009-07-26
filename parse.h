#ifndef _CANONICAL_H
#define _CANONICAL_H

#define NSYMS 256

struct symtab {
	char *name;
	char val; /* boolean! */
} symtab[NSYMS];

enum bool_type { VAR, OR_EXPR, AND_EXPR, NOT_EXPR, PAREN_EXPR };

struct bool {
	enum bool_type type;
	union {
		struct {
			struct symtab *sym;
		} var;

		struct {
			struct bool *l;
			struct bool *r;
		} and, or;

		struct {
			struct bool *b;
		} not, paren;
	} u;
};

struct symtab * sym(char *s);
void            print_tree(struct bool *t);

#endif
