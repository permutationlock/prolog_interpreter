/*
 * main.cpp
 * Author: Aven Bross (based on code by Chris Hartman)
 * Date: 3/14/2015
 * Simple yacc expression parser
 * Uses flex + bison output from files lex.lpp and yacc.ypp
 */

#include <iostream>
using std::cout;

int yyrestart(FILE *in);
int yyparse();

extern "C" int yywrap()
{
    cout << "End of input\n";
    yyparse();
    return 0;
}

int main(int argc, const char * argv[]) {
    yyparse();
}
