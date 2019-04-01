/*!
 * \file ImplyOp.cpp
 * \brief implementation of the imply logical operation
 * \author S.B
 * \date 21/03/2018
 */

#include "Operation.hpp"

#include <iostream>

using namespace std;

/*----------------------------------------------------------------------------*/
ImplyOp::ImplyOp(Term* left, Term* right):
Operation(left->expression())
{
    _terms.push_back(left);
    _terms.push_back(right);
    _op = Operation::Operator::Imply;
}

/*----------------------------------------------------------------------------*/
string ImplyOp::toString() {
    string res = "(";
    res += _terms.at(0)->toString();
    res += " -> ";
    res += _terms.at(1)->toString();
    res += ")";
    return res;
}

/*----------------------------------------------------------------------------*/
void ImplyOp::toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending) {

    if(polarity) {
        new cnf::Clause({createCnfLiteral(cnfVar, false), _terms.at(0)->createCnfLiteral(cnfVar, false), _terms.at(1)->createCnfLiteral(cnfVar, true)});
        updatePendingCnf(processed, pending, _terms.at(0), 1);
        updatePendingCnf(processed, pending, _terms.at(1), 2);
    } else {

        new cnf::Clause({createCnfLiteral(cnfVar, true), _terms.at(0)->createCnfLiteral(cnfVar, true)});
        new cnf::Clause({createCnfLiteral(cnfVar, true), _terms.at(1)->createCnfLiteral(cnfVar, false)});

        updatePendingCnf(processed, pending, _terms.at(0), 2);
        updatePendingCnf(processed, pending, _terms.at(1), 1);
    }

}

/*----------------------------------------------------------------------------*/
void ImplyOp::toCnfRoot(vector<cnf::Variable*>& cnfVar, vector<std::pair<bool, bool>>& processed, stack<pair<Operation*, bool>>& pending) {
    /* one clause : not a or b */
    new cnf::Clause({_terms.at(0)->createCnfLiteral(cnfVar, false), _terms.at(1)->createCnfLiteral(cnfVar, true)});
    updatePendingCnf(processed, pending, _terms.at(0), 1);
    updatePendingCnf(processed, pending, _terms.at(1), 2);
}
