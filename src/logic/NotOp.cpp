/*!
 * \file NotOp.cpp
 * \brief implementation of the not logical operation
 * \author S.B
 * \date 21/03/2018
 */

#include "Operation.hpp"

#include <iostream>

using namespace std;

/*----------------------------------------------------------------------------*/
NotOp::NotOp(Term* term):
Operation(term->expression())
{
    _terms.push_back(term);
    _op = Operation::Operator::Not;
}

/*----------------------------------------------------------------------------*/
string NotOp::toString() {
    string res = "not(" + _terms.front()->toString() + ")";
    return res;
}

/*----------------------------------------------------------------------------*/
void NotOp::toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending) {
    if(_terms.at(0)->type() == Term::Type::Operation) {
        updatePendingCnf(processed, pending, (!polarity ? 2 : 1));
    }
}

/*----------------------------------------------------------------------------*/
cnf::Literal NotOp::createCnfLiteral(vector<cnf::Variable*>& cnfVar, bool polarity) {
    /* simply invert the polarity of the sub term */
    return cnf::Literal(cnfVar.at(_terms.at(0)->index()), !polarity);
}

/*----------------------------------------------------------------------------*/
void NotOp::toCnfRoot(vector<cnf::Variable*>& cnfVar, vector<std::pair<bool, bool>>& processed, stack<pair<Operation*, bool>>& pending) {

    /* one clause : not a */
    new cnf::Clause({_terms.at(0)->createCnfLiteral(cnfVar, false)});
    updatePendingCnf(processed, pending, 1);
}
