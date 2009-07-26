#ifndef _SYMBOL_H
#define _SYMBOL_H

#define NSYMS 256

struct symtab {
	char *name;
	char val; /* boolean! */
};

enum bool_type { VAR, OR_EXPR, AND_EXPR, NOT_EXPR, PAREN_EXPR };

struct bool {
	enum bool_type type;
	union {
		struct {
			struct symtab *sym;
			int i;
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

struct symtab   *symbol(char *s);
void            print_tree(struct bool *t);

#endif
