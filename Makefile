CC   = gcc
CFLAGS = -g
LEX    = flex
YACC   = bison
YFLAGS = -vtd

BIN  = canonical

SRCS = $(BIN).y $(BIN).l
OBJS = lex.yy.o $(BIN).tab.o
LIBS = -lfl

$(BIN): $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) $(LIBS) -o $(BIN)

$(BIN).tab.h $(BIN).tab.c: $(BIN).y
	$(YACC) $(YFLAGS) $(BIN).y  

lex.yy.c: $(BIN).l $(BIN).tab.h
	$(LEX) $(BIN).l  # -d debug

all:    
	touch $(SRCS)
	make

clean:
	rm -f $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).tar

tar:
	tar -cvf $(BIN).tar $(SRCS) Makefile 
