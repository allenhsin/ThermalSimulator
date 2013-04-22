
parser := verilog
class := verilog

all: snazzle

snazzle.tab.c snazzle.tab.h: snazzle.y
	bison -p $(parser) -d snazzle.y

lex.$(parser).c: snazzle.l snazzle.tab.h
	flex -P$(parser) snazzle.l

snazzle: lex.$(parser).c snazzle.tab.c snazzle.tab.h
	g++ snazzle.tab.c lex.$(parser).c -lfl -o snazzle
    
clean:
	rm -rf snazzle.exe location.h stack.h position.h \
        snazzle.tab.c snazzle.tab.h lex.$(parser).c