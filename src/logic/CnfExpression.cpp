/*!
 * \file CnfExpression.cpp
 * \brief CNF Expression classes implementation (representing a CNF logical formula)
 * \author S.B
 * \date 21/03/2018
 */

#include <fstream>
#include <iostream>

#include "CnfExpression.hpp"

using namespace std;
using namespace cnf;

/******************************************************************************/
/*------------------------- class CnfExpression ------------------------------*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
CnfExpression::CnfExpression()
{

}

/*----------------------------------------------------------------------------*/
CnfExpression::CnfExpression(string fileName) {

}

/*----------------------------------------------------------------------------*/
int CnfExpression::addVariable(Variable* var) {
    _variables.push_back(var);
    return static_cast<int>(_variables.size()-1);
}

/*----------------------------------------------------------------------------*/
void CnfExpression::addClause(Clause* clause) {
    _clauses.push_back(clause);
}

/*----------------------------------------------------------------------------*/
int CnfExpression::nVar() {
    return static_cast<int>(_variables.size());
}

/*----------------------------------------------------------------------------*/
int CnfExpression::nClause() {
    return static_cast<int>(_clauses.size());
}

/*----------------------------------------------------------------------------*/
string CnfExpression::toString() {
    string res = "";
    for(auto clause : _clauses) {
        res += clause->toString() + "\n";
    }
    return res;
}

/*----------------------------------------------------------------------------*/
void CnfExpression::exportDimacs(std::string fileName) {
    ofstream file(fileName);
    if(file) {
        /* first line : number of variables and number of clauses */
        file << "p cnf " + to_string(_variables.size()) + " " + to_string(_clauses.size()) + "\n";
        /* clauses */
        for(auto cl : _clauses) {
            for(auto lit : cl->literals()) {
                if(!lit.positive()) {
                    file << "-";
                }
                file << to_string(lit.variable().index()+1) << " ";
            }
            file << "0\n";
        }
        file.close();
    }
}

/*----------------------------------------------------------------------------*/
CnfExpression::~CnfExpression() {

    for(auto clause : _clauses) {
        delete clause;
    }

    for(auto var : _variables) {
        delete var;
    }

}


/******************************************************************************/
/*---------------------------- class Variable --------------------------------*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
Variable::Variable(CnfExpression& ex):
_ex(ex)
{
    _ind = _ex.addVariable(this);
}

/*----------------------------------------------------------------------------*/
string Variable::toString() {
    return "x_" + to_string(_ind);
}

/*----------------------------------------------------------------------------*/
CnfExpression& Variable::expression() {
    return _ex;
}

/*----------------------------------------------------------------------------*/
int Variable::index() {
    return _ind;
}



/******************************************************************************/
/*---------------------------- class Literal ---------------------------------*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
Literal::Literal(Variable* var, bool pos):
_var(*var),
_pos(pos)
{

}

/*----------------------------------------------------------------------------*/
string Literal::toString() {
    string res = "";
    if(!_pos) {
        res += "not ";
    }
    res += _var.toString();
    return res;
}

/*----------------------------------------------------------------------------*/
Variable& Literal::variable() {
    return _var;
}

/*----------------------------------------------------------------------------*/
bool Literal::positive() {
    return _pos;
}





/******************************************************************************/
/*----------------------------- class Clause ---------------------------------*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
Clause::Clause(std::vector<Literal> lit):
_lit(lit)
{
    _lit.front().variable().expression().addClause(this);
}

/*----------------------------------------------------------------------------*/
string Clause::toString() {
    string res = "(";
    for(unsigned int i = 0; i < _lit.size(); i++) {
        res += _lit.at(i).toString();
        if(i < _lit.size()-1) {
            res += " or ";
        }
    }
    res += ")";
    return res;
}

/*----------------------------------------------------------------------------*/
vector<Literal>& Clause::literals() {
    return _lit;
}
