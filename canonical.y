%{
static int			 lineno = 1;
static int			 errors = 0;
const char			*infile;

//int yydebug = 1;

int	yyerror(char *str);
int	yyparse(void);
int	yylex(void);

%}

%token	<s> TERM
%token  AND OR
%token  LP RP
%token  NOT
%token  WS

%type <s> query expr

%right NOT
%left  OR
%left  AND WS

%union {
	char s[2048];
}

%%

query   : { /* nothing */ }
        | expr {
		printf("%s\n", $$);
        }
;

expr    : TERM                { sprintf($$, "%s", $1);         }
        | expr AND expr       { sprintf($$, "%s*%s", $1, $3);    }
        | expr WS expr        { sprintf($$, "%s*%s", $1, $3);    }
        | expr OR expr        { sprintf($$, "%s+%s", $1, $3);    }
        | NOT expr            { sprintf($$, "-%s", $2);        }
        | LP expr RP          { sprintf($$, "(%s)", $2);         }
;

%%

int
yyerror(char *str)
{
	printf("%s\n", str);
}

int
main(int argc, char *argv)
{
	yyparse();

	return (errors ? -1 : 0);
}

