# Specify compiler
COMP = clang++ -std=c++1y -O2

# Specify target
all: aven_prolog

# Build interpreter executable
aven_prolog: main.cpp unification/unification.cpp yacc.tab.cpp lex.yy.c
	$(COMP) main.cpp yacc.tab.cpp lex.yy.c unification/unification.cpp -o prolog

# Setup bison file
yacc.tab.cpp: yacc.ypp
	bison -d yacc.ypp

# Setup flex file
lex.yy.c: lex.lpp
	flex lex.lpp

# Clean build
clean:
	rm lex.yy.c yacc.tab.cpp yacc.tab.hpp prolog

