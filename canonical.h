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


/* struct onion { */
/*         in_addr_t               o_addr; */
/*         in_port_t               o_port; */
/*         u_int16_t               o_flags; */
/*         char                    o_opts[6]; */
/*         union { */
/*                 char            os_layer[1]; */
/*                 struct cryptbuf os_crypt; */
/*         } ou_layer; */
/* }; */
/* #define o_layer ou_layer.os_layer */
/* #define o_hdr   ou_layer.os_crypt.c_hdr */
/* #define o_data  ou_layer.os_crypt.c_data */
