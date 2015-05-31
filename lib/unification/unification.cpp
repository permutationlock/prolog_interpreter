/*
 * unification.cpp
 * Author: Aven Bross
 * Date: 5/7/2015
 * Code for prolog unification.  For use in conjunction
 * with lex/yacc
 */

#ifndef UNIFICATION_CPP
#define UNIFICATION_CPP

#include "unification.h"

// Pointer construction functions for yacc

Term * newAtom(const string & name){
    Term * term = new Term;
    term -> type = 'a';
    term -> name = name;
    return term;
}

Term * newNumber(int num){
    Term * term = new Term;
    term -> type = 'n';
    term -> number = num;
    return term;
}

Term * newVariable(const string & name){
    Term * term = new Term;
    term -> type = 'v';
    term -> name = name;
    return term;
}

Term * newComplex(const string & name, const vector<Term> & subterms){
    Term * term = new Term;
    term -> type = 'c';
    term -> name = name;
    term -> subterms = vector<Term>(subterms);
    return term;
}

Clause * newFact(const Term & term){
    Clause * clause = new Clause;
    clause -> type = 'f';
    clause -> term = term;
    return clause;
}

Clause * newRule(const Term & term, const vector<Clause> & subclauses){
    Clause * clause = new Clause;
    clause -> type = 'r';
    clause -> term = term;
    clause -> subclauses = vector<Clause>(subclauses);
    return clause;
}

TermList * newTermList(){
    TermList * list = new TermList;
    return list;
}

ClauseList * newClauseList(){
    ClauseList * list = new ClauseList;
    return list;
}

// Print clause
void print(const Clause & clause){
    print(clause.term);
    if(clause.type == 'r'){
        cout << " :- ";
        for(auto subclause : clause.subclauses){
            print(subclause);
            cout << ", ";
        }
    }
}

// Print term
void print(const Term & term){
    if(term.type == 'n'){
        cout << term.number;
    }
    else{
        cout << term.name;
    }
    if(term.type == 'c'){
        cout << "(";
        for(int i=0; i<term.subterms.size(); i++){
            print(term.subterms[i]);
            if(i<term.subterms.size()-1){
                cout << ", ";
            }
        }
        cout << ")";
    }
}

// Rewrite all variables in a clause to unique variable names
void rewriteVars(Clause & clause, int & count){
    map<string,string> m;
    rewriteVars(m, clause, count);
}

// Rewrite all variables in a clause to unique variables names, 
// tracking substitutions with given substitution map
void rewriteVars(map<string,string> & m, Clause & clause, int & count){
    rewriteVars(m,clause.term, count);
    for(auto & subclause : clause.subclauses){
        rewriteVars(m,subclause, count);
    }
}

// Rewrite all variables in a term to unique variables names, 
// tracking substitutions with given substitution map
void rewriteVars(map<string,string> & m, Term & term, int & count){
    if(term.type == 'v'){
        if(m.count(term.name) != 1){
            m[term.name] = string("%") + term.name + "_" + to_string(count);
            count++;
        }
        term.name = m[term.name];
    }
    else if(term.type == 'c'){
        for(auto & subterm : term.subterms){
            rewriteVars(m,subterm, count);
        }
    }
}

// Go through clause and make variable substitutions according
// to substitution map
void substitute(map<string,Term> & m, Clause & clause){
    substitute(m,clause.term);
    // Otherwise we have a rule with subclauses
    for(auto & subclause : clause.subclauses){
        substitute(m,subclause);
    }
}

// Go through term and make variable substitutions according
// to substitution map
void substitute(map<string,Term> & m, Term & term){
    if(term.type == 'v' && m.count(term.name) == 1){
        term = m[term.name];
    }
    else if(term.type == 'c'){
        for(auto & subterm : term.subterms){
            substitute(m, subterm);
        }
    }
}

// Takes a list of clauses and variables cared about
// Displays information about their truthfulness
void doQuery(vector<Clause> & querylist, const vector<string> & variables, const vector<Clause> & knowledgebase){
    bool success = false;
    bool var_found = false;
    try{
        vector<map<string,Term>> results = search(querylist, knowledgebase);
        for(int i=0; i<results.size(); i++){
            map<string,Term> m = results[i];
            for(const auto & var : variables){
                string temp = var;
                while(m.count(temp) == 1 ){
                    Term term = m[temp];
                    if(term.type == 'v' ){
                        temp = term.name;
                    }
                    else{
                        cout << var << " = ";
                        print(term);
                        cout << "\n";
                        var_found = true;
                        break;
                    }
                }
            }
            if(var_found && i<results.size()-1)
                cout << ";\n";
        }
        success = true;
    }
    catch(logic_error e){
        
    }
    if(!var_found){
        if(success) cout << "yes.\n";
        else cout << "no.\n";
    }
}

// Attempts to unify the clause list with each clause in the database
// Returns a vector of each substitution map that works
vector<map<string,Term>> search(vector<Clause> & querylist, const vector<Clause> & knowledgebase){
    Clause query = querylist.front();
    vector<map<string,Term>> result;
    
    bool success = false;
    for(const auto & clause : knowledgebase){
        vector<Clause> newlist(querylist.begin()+1,querylist.end());
        try{
            vector<map<string,Term>> mlist = unify(query,clause, knowledgebase);
            
            if(newlist.size()>0){
                for(auto m : mlist){
                    for(auto & other : newlist){
                        substitute(m,other);
                    }
                    
                    vector<map<string,Term>> temp = search(newlist, knowledgebase);
                    for(auto & tm : temp){
                        for(auto & s : m){
                            tm[s.first] = s.second; 
                        }
                        result.push_back(tm);
                    }
                }
            }
            else{
                for(auto m : mlist){
                    result.push_back(m);
                }
            }
            success = true;
        }
        catch(logic_error e){
            
        }
    }
    if(!success){
        throw logic_error("Failed Unification");
    }
    return result;
}

// Attempts to unify two clauses
// Returns a vector of substitution maps, representing substitutions made
vector<map<string,Term>> unify(Clause & clause1, Clause clause2, const vector<Clause> & knowledgebase){
    vector<map<string,Term>> result;
    if(clause2.type == 'f'){
        result.push_back(unify(clause1.term,clause2.term));
        
    }
    else{
        map<string,Term> m = unify(clause1.term,clause2.term);
        substitute(m,clause2);
        vector<map<string,Term>> temp = search(clause2.subclauses, knowledgebase);
        for(auto & tm : temp){
            for(auto & s : m){
                tm[s.first] = s.second; 
            }
            result.push_back(tm);
        }
    }
    
    return result;
}

// Attempts to unify two terms
// Returns a substitution map of any variable substitutions made
map<string,Term> unify(Term & term1, Term & term2){
    // Dealing with two non terms of the same type
    if(term2.type == term1.type){
        // Two constants unify if they have the same name
        if(term1.type == 'a' && term2.name.compare(term1.name) == 0){
            return map<string,Term>();
        }
        // Two numbers unify if they have the same value
        else if(term1.type == 'n' && term2.number == term1.number){
            return map<string,Term>();
        }
        else if(term1.type == 'v'){
            map<string,Term> m;
            
            // Only add substitution if different variables
            if(term1.name != term2.name){
                if(m.count(term1.name) == 1){
                    m[term2.name] = m[term1.name];
                }
                else if(m.count(term2.name) == 1){
                    m[term1.name] = m[term2.name];
                }
                else{
                    m[term1.name] = term2;
                }
            }
            return m;
        }
        // Two complex terms unify if their subterms unify
        else if(term1.type == 'c' && term2.name == term1.name && 
                term2.subterms.size() == term1.subterms.size()){
            map<string,Term> m;
            for(int i=0; i<term1.subterms.size(); i++){
                substitute(m, term1.subterms[i]);
                substitute(m, term2.subterms[i]);
                map<string,Term> temp = unify(term1.subterms[i],term2.subterms[i]);
                for(auto & p : temp){
                    m[p.first] = p.second;
                }
            }
            return m;
        }
    }
    else if(term1.type == 'v'){
        map<string,Term> m;
        m[term1.name] = term2;
        return m;
    }
    else if(term2.type == 'v'){
        map<string,Term> m;
        m[term2.name] = term1;
        return m;
    }
    throw logic_error("Failed Unification");
    return map<string,Term>();
}

#endif
