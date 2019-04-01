/*!
 * \file OrOp.cpp
 * \brief implementation of the or logical operation
 * \author S.B
 * \date 21/03/2018
 */

#include "Operation.hpp"

#include <iostream>
#include <list>

using namespace std;

/*----------------------------------------------------------------------------*/
OrOp::OrOp(vector<Term*> terms):
Operation(terms.front()->expression())
{
    _terms = terms;
    _op = Operation::Operator::Or;
}

/*----------------------------------------------------------------------------*/
string OrOp::toString() {
    string res = "(";
    for(unsigned int ind = 0; ind < _terms.size(); ind ++) {
        res += _terms.at(ind)->toString();
        if(ind < _terms.size()-1) {
            res += " or ";
        }
    }
    res += ")";
    return res;
}

/*----------------------------------------------------------------------------*/
bool OrOp::reduce(stack<Term*>& pending) {

    bool modified = false;

    list<vector<Term*>::iterator> toRemove;
    list<Term*> toAdd;

    for(auto it = _terms.begin(); it != _terms.end(); it ++) {
        if((*it)->type() == Term::Type::Operation) {

            Operation* op = dynamic_cast<Operation*>(*it);

            if(op->operatorType() == Operation::Operator::Or && op->nRef() <= 1) {
                modified = true;
                toRemove.push_back(it);
                toAdd.insert(toAdd.end(), op->terms().begin(), op->terms().end());
            }
        }
    }

    for(auto it : toRemove) {
        Operation* op = static_cast<Operation*>(*it);
        _terms.erase(it);
        _ex.operations().at(op->opIndex()) = nullptr;
        _ex.terms().at(op->index()) = nullptr;
        delete op;
    }

    if(!toAdd.empty()) {
        _terms.insert(_terms.end(), toAdd.begin(), toAdd.end());
    }

    if(!modified) {
        for(auto term : _terms) {
            pending.push(term);
        }
    }

    return modified;
}

/*----------------------------------------------------------------------------*/
void OrOp::toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending) {

    if(polarity) {

        vector<cnf::Literal> litList;
        litList.push_back(createCnfLiteral(cnfVar, false));
        for(auto term : _terms) {
            litList.push_back(term->createCnfLiteral(cnfVar, true));
        }

        new cnf::Clause(litList);

        /* add the remaining transformations if necessary (positive) */
        updatePendingCnf(processed, pending, 2);

    } else {

        /* one literal per sub term */
        for(auto term : _terms) {
            new cnf::Clause({createCnfLiteral(cnfVar, true), term->createCnfLiteral(cnfVar, false)});
        }

        /* add the remaining transformations if necessary (negative) */
        updatePendingCnf(processed, pending, 1);
    }

}

/*----------------------------------------------------------------------------*/
void OrOp::toCnfRoot(vector<cnf::Variable*>& cnfVar, vector<std::pair<bool, bool>>& processed, stack<pair<Operation*, bool>>& pending) {

    vector<cnf::Literal> litList;
    /* only one clause */
    for(Term* term : _terms) {
        litList.push_back(term->createCnfLiteral(cnfVar, true));
    }

    new cnf::Clause(litList);

    updatePendingCnf(processed, pending, 2);

}
