/*!
 * \file Operation.cpp
 * \brief implementation of class Operation
 * \author S.B
 * \date 20/03/2018
 */

#include "Operation.hpp"

#include <iostream>

using namespace std;

/*----------------------------------------------------------------------------*/
Operation::Operation(Expression& ex) :
Term(ex),
_op(Operator::None),
_nRef(0)
{
    _type = Term::Operation;
    auto inds = _ex.addOperation(this);
    _ind = inds.first;
    _opInd = inds.second;
}

/*----------------------------------------------------------------------------*/
vector<Term*>& Operation::terms() {
    return _terms;
}

/*----------------------------------------------------------------------------*/
Operation::Operator Operation::operatorType() {
    return _op;
}

/*----------------------------------------------------------------------------*/
int Operation::opIndex() {
    return _opInd;
}

/*----------------------------------------------------------------------------*/
void Operation::setOpIndex(int opInd) {
    _opInd = opInd;
}

/*----------------------------------------------------------------------------*/
bool Operation::reduce(stack<Term*>& pending) {
    for(auto term : _terms) {
        pending.push(term);
    }
    return false;
}

/*----------------------------------------------------------------------------*/
void Operation::updatePendingCnf(vector<pair<bool, bool>>& processed, stack<pair<Operation*, bool>>& pending, int pol) {

    for(auto term : _terms) {
        updatePendingCnf(processed, pending, term, pol);
    }

}

/*----------------------------------------------------------------------------*/
void Operation::updatePendingCnf(vector<pair<bool, bool>>& processed, stack<pair<Operation*, bool>>& pending, Term* term, int pol) {
    if(term->type() == Term::Type::Operation) {
        Operation* op = dynamic_cast<Operation*>(term);

        /* negative operations */
        if((pol == 0 || pol == 1) && !processed.at(op->opIndex()).first) {
            processed.at(op->opIndex()).first = true;
            pending.push(pair<Operation*, bool>(op, false));
        }

        /* positive operations */
        if((pol == 0 || pol == 2) && !processed.at(op->opIndex()).second) {
            processed.at(op->opIndex()).second = true;
            pending.push(pair<Operation*, bool>(op, true));
        }

    }
}

/*----------------------------------------------------------------------------*/
short int Operation::nRef() {
    return _nRef;
}

/*----------------------------------------------------------------------------*/
void Operation::increaseRef() {
    _nRef = static_cast<short int>(1+_nRef);
}

/*----------------------------------------------------------------------------*/
void Operation::updateRef() {
    for(Term* term : _terms) {
        if(term->type() == Term::Type::Operation) {
            dynamic_cast<Operation*>(term)->increaseRef();
        }
    }
}
