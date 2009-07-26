CC   = gcc
CFLAGS = -g
LIBS = -lfl
LEX    = flex
YACC   = bison
YFLAGS = -vtd

all: canonical

canonical: lex.yy.o canonical.tab.o truth.o
	$(CC) $(CCFLAGS) $(LIBS) lex.yy.o canonical.tab.o truth.o -o canonical

truth.o: truth.c truth.h
	$(CC) $(CCFLAGS) -c truth.c

canonical.tab.h canonical.tab.c: canonical.y parse.h truth.h
	$(YACC) $(YFLAGS) canonical.y  

canonical.tab.o: canonical.tab.c
	$(CC) $(CCFLAGS) -c canonical.tab.c	

lex.yy.c: canonical.l canonical.tab.h parse.h
	$(LEX) canonical.l  # -d debug

lex.yy.o: lex.yy.c
	$(CC) $(CCFLAGS) -c lex.yy.c



clean:
	rm -f canonical truth *.o canonical.tab.{c,h} lex.yy.c canonical.output

