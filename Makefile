CC = cc
FLEX_FILE = lexer.l
BISON_FILE = yaccer.y
OBJECTS = lex.yy.c y.tab.c

parser: $(OBJECTS)
	$(CC) $(OBJECTS) -o parser -lfl -ly

y.tab.c: $(BISON_FILE)
	bison -d $(BISON_FILE) -y

lex.yy.c: $(FLEX_FILE)
	flex $(FLEX_FILE)

clean:
	rm $(OBJECTS) y.tab.h

clear:
	rm parser
