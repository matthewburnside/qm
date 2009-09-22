CC	= gcc
CFLAGS	= -g
LIBS	=
LEX	= flex
YACC	= bison
YFLAGS	= -vtd

all: qm

qm : lex.yy.c parse.tab.c parse.tab.h qm.c qm.h truth.c truth.h
	$(CC) $(CFLAGS) $(LIBS) -o qm lex.yy.c parse.tab.c qm.c truth.c

parse.tab.h parse.tab.c: parse.y parse.h truth.h
	$(YACC) $(YFLAGS) parse.y  
 
lex.yy.c: lex.l parse.tab.h parse.h
	$(LEX) lex.l

tags:
	ctags -R .
clean:
	rm -f qm truth *.o parse.tab.{c,h} lex.yy.c parse.output
