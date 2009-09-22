#ifndef _SYMBOL_H
#define _SYMBOL_H

#define NSYMS 256

struct symtab {
    char    *name;
};

enum expr_type { VAR, OR_EXPR, AND_EXPR, NOT_EXPR, PAREN_EXPR };

struct expr {
    enum expr_type type;
    union {
        struct {
            int sym;
        } var;
        struct {
            struct expr *l;
            struct expr *r;
        } and, or;
        struct {
            struct expr *b;
        } not, paren;
    } u;
};

int				symbol(char *s);
void            print_tree(struct expr *t);
struct expr *   new_expr(enum expr_type type);

#endif
