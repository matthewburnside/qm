#ifndef _SYMBOL_H
#define _SYMBOL_H

#define NSYMS 256

struct symtab {
	char *name;
};

enum bool_type { VAR, OR_EXPR, AND_EXPR, NOT_EXPR, PAREN_EXPR };

struct bool {
	enum bool_type type;
	union {
		struct {
			int sym;
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

int             symbol(char *s);
void            print_tree(struct bool *t);

#endif
