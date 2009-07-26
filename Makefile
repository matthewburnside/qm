CC   = gcc
CFLAGS = -g
LIBS = -lfl
LEX    = flex
YACC   = bison
YFLAGS = -vtd

all: canonical

canonical: lex.yy.o canonical.tab.o
	$(CC) $(CCFLAGS) $(LIBS) lex.yy.o canonical.tab.o -o canonical

canonical.tab.h canonical.tab.c: canonical.y
	$(YACC) $(YFLAGS) canonical.y  

lex.yy.c: canonical.l canonical.tab.h
	$(LEX) canonical.l  # -d debug

clean:
	rm -f canonical *.o canonical.tab.{c,h} lex.yy.c canonical.output

