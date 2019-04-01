/*!
 * \file EquivalentOp.cpp
 * \brief implementation of the equivalent logical operation
 * \author S.B
 * \date 21/03/2018
 */

#include "Operation.hpp"

#include <iostream>

using namespace std;

/*----------------------------------------------------------------------------*/
EquivalentOp::EquivalentOp(Term* left, Term* right):
Operation(left->expression())
{
    _terms.push_back(left);
    _terms.push_back(right);
    _op = Operation::Operator::Equivalent;
}

/*----------------------------------------------------------------------------*/
string EquivalentOp::toString() {
    string res = "(";
    res += _terms.at(0)->toString();
    res += " <-> ";
    res += _terms.at(1)->toString();
    res += ")";
    return res;
}

/*----------------------------------------------------------------------------*/
void EquivalentOp::toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending) {

    if(polarity) {
        new cnf::Clause({createCnfLiteral(cnfVar, false), _terms.at(0)->createCnfLiteral(cnfVar, false), _terms.at(1)->createCnfLiteral(cnfVar, true)});
        new cnf::Clause({createCnfLiteral(cnfVar, false), _terms.at(0)->createCnfLiteral(cnfVar, true), _terms.at(1)->createCnfLiteral(cnfVar, false)});
    } else {
        new cnf::Clause({createCnfLiteral(cnfVar, true), _terms.at(0)->createCnfLiteral(cnfVar, true), _terms.at(1)->createCnfLiteral(cnfVar, true)});
        new cnf::Clause({createCnfLiteral(cnfVar, true), _terms.at(0)->createCnfLiteral(cnfVar, false), _terms.at(1)->createCnfLiteral(cnfVar, false)});
    }

    updatePendingCnf(processed, pending, 2);
}

/*----------------------------------------------------------------------------*/
void EquivalentOp::toCnfRoot(vector<cnf::Variable*>& cnfVar, vector<std::pair<bool, bool>>& processed, stack<pair<Operation*, bool>>& pending) {

    /* two clauses : (not a or b) and (not b or a) */
    new cnf::Clause({_terms.at(0)->createCnfLiteral(cnfVar, false), _terms.at(1)->createCnfLiteral(cnfVar, true)});
    new cnf::Clause({_terms.at(0)->createCnfLiteral(cnfVar, true), _terms.at(1)->createCnfLiteral(cnfVar, false)});

    updatePendingCnf(processed, pending, 0);
}
