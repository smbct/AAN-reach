/*!
 * \file Term.cpp
 * \brief Term class implementation (representing a term in a logical formula)
 * \author S.B
 * \date 14/03/2018
 */

#include "Term.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
Term::Term(Expression& ex) :
_ex(ex),
_type(Type::Undefined)
{

}

/*----------------------------------------------------------------------------*/
Term::Type Term::type() {
    return _type;
}

/*----------------------------------------------------------------------------*/
int Term::index() {
    return _ind;
}

/*----------------------------------------------------------------------------*/
void Term::setIndex(int index) {
    _ind = index;
}

/*----------------------------------------------------------------------------*/
Expression& Term::expression() {
    return _ex;
}

/*----------------------------------------------------------------------------*/
cnf::Literal Term::createCnfLiteral(vector<cnf::Variable*>& cnfVar, bool polarity) {
    return cnf::Literal(cnfVar.at(_ind), polarity);
}

/*----------------------------------------------------------------------------*/
Term::~Term() {

}
