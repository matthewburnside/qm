CC   = gcc
CFLAGS = -g
LIBS =
LEX    = flex
YACC   = bison
YFLAGS = -vtd

all: qm

qm: lex.yy.o parse.tab.o truth.o qm.o
	$(CC) $(CCFLAGS) $(LIBS) lex.yy.o parse.tab.o truth.o qm.o -o qm

truth.o: truth.c truth.h
	$(CC) $(CCFLAGS) -c truth.c

qm.o: qm.c qm.h
	$(CC) $(CCFLAGS) -c qm.c

parse.tab.h parse.tab.c: parse.y parse.h truth.h
	$(YACC) $(YFLAGS) parse.y  

parse.tab.o: parse.tab.c
	$(CC) $(CCFLAGS) -c parse.tab.c	

lex.yy.c: lex.l parse.tab.h parse.h
	$(LEX) lex.l  # -d debug

lex.yy.o: lex.yy.c
	$(CC) $(CCFLAGS) -c lex.yy.c

clean:
	rm -f qm truth *.o parse.tab.{c,h} lex.yy.c parse.output

