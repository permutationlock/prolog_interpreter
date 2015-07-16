/*
 * unification.h
 * Author: Aven Bross
 * Date: 5/7/2015
 * Code for prolog unification.  For use in conjunction
 * with lex/yacc
 */

#ifndef UNIFICATION_H
#define UNIFICATION_H

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <iostream>

using std::vector;
using std::string;
using std::map;
using std::logic_error;
using std::cout;
using std::to_string;

/* Term:
*    types - 'a' = atom
*            'n' = number
*            'v' = variable
*            'c' = complex term
*    arity - # of subterms
*    number - value of number term
*    name - name of variable, atom or complex term
*    subterms - array of subterms of complex term
*/

struct Term {
    char type;
    int number;
    string name;
    vector<Term> subterms;
};

struct TermList {
    vector<Term> terms;
    vector<string> variables;
};


/* Clause:
*    types - 'f' = fact
*            'r' = rule
*    fact - the fact
*    size - number of subfacts for rule
*    subfacts - array of subfacts for rule
*/

struct Clause {
    char type;
    Term term;
    vector<Clause> subclauses;
};

struct ClauseList {
    vector<Clause> clauses;
    vector<string> variables;
};

// Pointer construction functions for yacc
Term * newAtom(const string & name);
Term * newNumber(int num);
Term * newVariable(const string & name);
Term * newComplex(const string & name, const vector<Term> & subterms);
Clause * newFact(const Term & term);
Clause * newRule(const Term & term, const vector<Clause> & subclauses);
TermList * newTermList();
ClauseList * newClauseList();


// Print terms and clauses
void print(const Clause & clause);
void print(const Term & term);

// Recursive variable rewrite to avoid duplicate variables
void rewriteVars(Clause & clause, int & count);
void rewriteVars(map<string,string> & m, Clause & clause, int & count);
void rewriteVars(map<string,string> & m, Term & term, int & count);

// Recursive variables substitute with given substitution map
void substitute(map<string,Term> & m, Clause & clause);
void substitute(map<string,Term> & m, Term & term);

// Takes a list of clauses and variables cared about and knowledgebase being searched
// Displays information about their truthfulness
void doQuery(vector<Clause> & querylist, const vector<string> & variables, const vector<Clause> & knowledgebase);

// Attempts to unify the clause list with each clause in the knowledgebase
// Returns a vector of each substitution map that works
vector<map<string,Term>> search(vector<Clause> & querylist, const vector<Clause> & knowledgebase);

// Attempts to unify two clauses in the given knowledgebase
// Returns a vector of substitution maps, representing substitutions made
vector<map<string,Term>> unify(Clause & clause1, Clause clause2, const vector<Clause> & knowledgebase);

// Attempts to unify two terms
// Returns a substitution map of any variable substitutions made
map<string,Term> unify(Term & term1, Term & term2);

#endif
