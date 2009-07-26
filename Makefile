CC   = gcc
CFLAGS = -g
LIBS = -lfl
LEX    = flex
YACC   = bison
YFLAGS = -vtd

all: canonical truth

truth: lex.yy.o canonical.tab.o truth.o parse.h
	$(CC) $(CCFLAGS) $(LIBS) lex.yy.o canonical.tab.o -o truth

canonical: lex.yy.o canonical.tab.o parse.h
	$(CC) $(CCFLAGS) $(LIBS) lex.yy.o canonical.tab.o -o canonical

canonical.tab.h canonical.tab.c: canonical.y parse.h
	$(YACC) $(YFLAGS) canonical.y  

lex.yy.c: canonical.l canonical.tab.h parse.h
	$(LEX) canonical.l  # -d debug

clean:
	rm -f canonical truth *.o canonical.tab.{c,h} lex.yy.c canonical.output

