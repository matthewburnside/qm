CC   = gcc
CFLAGS = -g
LIBS =
LEX    = flex
YACC   = bison
YFLAGS = -vtd

all: minbool

minbool: lex.yy.o minbool.tab.o truth.o
	$(CC) $(CCFLAGS) $(LIBS) lex.yy.o minbool.tab.o truth.o -o minbool

truth.o: truth.c truth.h
	$(CC) $(CCFLAGS) -c truth.c

minbool.tab.h minbool.tab.c: minbool.y parse.h truth.h
	$(YACC) $(YFLAGS) minbool.y  

minbool.tab.o: minbool.tab.c
	$(CC) $(CCFLAGS) -c minbool.tab.c	

lex.yy.c: lex.l minbool.tab.h parse.h
	$(LEX) lex.l  # -d debug

lex.yy.o: lex.yy.c
	$(CC) $(CCFLAGS) -c lex.yy.c

clean:
	rm -f minbool truth *.o minbool.tab.{c,h} lex.yy.c minbool.output

