/*!
 * \file AndOp.cpp
 * \brief implementation of the and logical operation
 * \author S.B
 * \date 20/03/2018
 */

#include "Operation.hpp"

#include <iostream>

using namespace std;

/*----------------------------------------------------------------------------*/
AndOp::AndOp(vector<Term*> terms):
Operation(terms.front()->expression())
{
    _terms = terms;
    _op = Operation::Operator::And;
}

/*----------------------------------------------------------------------------*/
string AndOp::toString() {
    string res = "(";
    for(unsigned int ind = 0; ind < _terms.size(); ind ++) {
        res += _terms.at(ind)->toString();
        if(ind < _terms.size()-1) {
            res += " and ";
        }
    }
    res += ")";
    return res;
}

/*----------------------------------------------------------------------------*/
void AndOp::toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending) {

    if(polarity) {

        /* one clause per sub term */
        for(auto term : _terms) {
            new cnf::Clause({createCnfLiteral(cnfVar, false), term->createCnfLiteral(cnfVar, true)});
        }

        /* add the remaining transformations if necessary (positive) */
        updatePendingCnf(processed, pending, 2);

    } else {

        /* only one clause */
        vector<cnf::Literal> litList;
        litList.push_back(createCnfLiteral(cnfVar, true));
        for(auto term : _terms) {
            litList.push_back(term->createCnfLiteral(cnfVar, false));
        }
        new cnf::Clause(litList);

        /* add the remaining transformations if necessary (negative) */
        updatePendingCnf(processed, pending, 1);
    }

}

/*----------------------------------------------------------------------------*/
void AndOp::toCnfRoot(vector<cnf::Variable*>& cnfVar, vector<std::pair<bool, bool>>& processed, stack<pair<Operation*, bool>>& pending) {

    /* one clause per sub term */

    for(Term* term : _terms) {
        new cnf::Clause({term->createCnfLiteral(cnfVar, true)});
    }
    updatePendingCnf(processed, pending, 2);
}
