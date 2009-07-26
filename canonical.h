#ifndef _CANONICAL_H
#define _CANONICAL_H

enum bool_type { VAR, OR_EXPR, AND_EXPR, NOT_EXPR, PAREN_EXPR };

struct bool {
	enum bool_type type;
	union {
		struct {
			char *name;
			int val;
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

#endif
