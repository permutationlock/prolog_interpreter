all: aven_prolog

aven_prolog: main.cpp unification/unification.cpp yacc.tab.cpp lex.yy.c
	g++ main.cpp yacc.tab.cpp lex.yy.c unification/unification.cpp -std=c++11 -o prolog

yacc.tab.cpp: yacc.ypp
	bison -d yacc.ypp

lex.yy.c: lex.lpp
	flex lex.lpp

clean:
	rm lex.yy.c yacc.tab.cpp yacc.tab.hpp prolog

