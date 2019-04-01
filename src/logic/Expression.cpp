/*!
 * \file Expression.cpp
 * \brief Expression class implementation
 * \author S.B
 * \date 14/03/2018
 */

#include "Expression.hpp"
#include "Term.hpp"
#include "Variable.hpp"
#include "Operation.hpp"

#include <iostream>
#include <stack>
#include <list>

#include "Operation.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
Expression::Expression() :
_term(nullptr)
{

}

/*----------------------------------------------------------------------------*/
void Expression::setMainTerm(Term* term) {
    _term = term;
}

/*----------------------------------------------------------------------------*/
int Expression::nVar() {
    return static_cast<int>(_variables.size());
}

/*----------------------------------------------------------------------------*/
pair<int,int> Expression::addVariable(Variable* var) {
    _variables.push_back(var);
    int termInd = addTerm(var);
    return pair<int,int>(termInd, static_cast<int>(_variables.size()-1));
}

/*----------------------------------------------------------------------------*/
pair<int,int> Expression::addOperation(Operation* op) {
    _operations.push_back(op);
    int termInd = addTerm(op);
    return pair<int,int>(termInd,static_cast<int>(_operations.size()-1));
}

/*----------------------------------------------------------------------------*/
int Expression::addTerm(Term* term) {
    _terms.push_back(term);
    return static_cast<int>(_terms.size()-1);
}

/*----------------------------------------------------------------------------*/
vector<Variable*>& Expression::variables() {
    return _variables;
}

/*----------------------------------------------------------------------------*/
vector<Operation*>& Expression::operations() {
    return _operations;
}

/*----------------------------------------------------------------------------*/
vector<Term*>& Expression::terms() {
    return _terms;
}

/*----------------------------------------------------------------------------*/
string Expression::toString() {
    if(_term == nullptr) {
        return "Undefined";
    } else {
        return _term->toString();
    }
}

/*----------------------------------------------------------------------------*/
OrOp* Expression::transformImply(ImplyOp* imply) {

    /* (a -> b) <-> (not a or b) */
    /* create "not a" */
    NotOp* newNot = new NotOp(imply->terms().at(0));
    OrOp* newOr = new OrOp({newNot, imply->terms().at(1)});

    _operations.at(imply->opIndex()) = nullptr;
    _terms.at(imply->index()) = nullptr;
    delete imply;

    return newOr;
}

/*----------------------------------------------------------------------------*/
void Expression::removeImply() {

    stack<Operation*> pending;

    /* transformation for the top term */
    if(_term->type() == Term::Type::Operation) {
        if(dynamic_cast<Operation*>(_term)->operatorType() == Operation::Operator::Imply) {
            _term = transformImply(dynamic_cast<ImplyOp*>(_term));
        }
        pending.push(dynamic_cast<Operation*>(_term));
    }

    /* transformation for sub operations */
    while(!pending.empty()) {

        Operation* top = pending.top();
        pending.pop();

        for(unsigned int ind = 0; ind < top->terms().size(); ind ++) {

            Term* term = top->terms().at(ind);
            if(term->type() == Term::Type::Operation) {
                Operation* op = dynamic_cast<Operation*>(term);
                if(op->operatorType() == Operation::Operator::Imply) {
                    OrOp* newOr = transformImply(dynamic_cast<ImplyOp*>(op));
                    top->terms().at(ind) = newOr;
                    pending.push(newOr);
                } else {
                    pending.push(op);
                }
            }
        }
    }


}

/*----------------------------------------------------------------------------*/
void Expression::updateOperationsPool() {

    _operations.clear();

    list<Term*> temp;
    for(auto term : _terms) {
        if(term != nullptr) {
            temp.push_back(term);
            term->setIndex(static_cast<int>(temp.size()-1));

            if(term->type() == Term::Type::Operation) {
                auto op = static_cast<Operation*>(term);
                _operations.push_back(op);
                op->setOpIndex(static_cast<int>(_operations.size()-1));
            }

        }
    }
    _terms.clear();
    _terms.insert(_terms.begin(), temp.begin(), temp.end());

}

/*----------------------------------------------------------------------------*/
void Expression::reduce() {

    /* tranform all the imply operations into or operations */
    removeImply();

    bool stop = false;
    stack<Term*> pending;
    pending.push(_term);
    bool modified = false;

    while(!stop) {

        Term* top = pending.top();
        pending.pop();

        if(top->type() == Term::Type::Operation) {
            if(dynamic_cast<Operation*>(top)->reduce(pending)) {
                modified = true;
            }
        }

        if(pending.empty()) {
            /* a fixed point has not been reached yet */
            if(modified) {
                pending.push(_term);
                modified = false;
            } else {
                stop = true;
            }
        }
    }

    /* update the operations list */
    updateOperationsPool();

}

/*----------------------------------------------------------------------------*/
void Expression::creatCnfVariables(cnf::CnfExpression& cnfEx, std::vector<cnf::Variable*>& cnfVar) {

    cnfVar.resize(_terms.size(), nullptr);

    stack<Term*> pending;

    if(_term->type() == Term::Type::Operation) {
        for(auto term : dynamic_cast<Operation*>(_term)->terms()) {
            pending.push(term);
        }
    }

    while(!pending.empty()) {

        Term* top = pending.top();
        pending.pop();

        if(cnfVar.at(top->index()) == nullptr) {

            if(top->type() == Term::Type::Operation) {
                auto op = dynamic_cast<Operation*>(top);
                if(op->operatorType() != Operation::Operator::Not) {
                    cnfVar.at(op->index()) = new cnf::Variable(cnfEx);
                }

                for(auto term : op->terms()) {
                    pending.push(term);
                }
            } else {
                cnfVar.at(top->index()) = new cnf::Variable(cnfEx);
            }

        }

    }
}

/*----------------------------------------------------------------------------*/
void Expression::topToCnf(vector<cnf::Variable*>& cnfVar, vector<pair<bool, bool>>& processed, stack<pair<Operation*, bool>>& pending) {
    if(_term->type() == Term::Type::Variable) {
        new cnf::Clause({cnf::Literal(cnfVar.at(_term->index()))});
    } else {
        Operation* op = dynamic_cast<Operation*>(_term);
        op->toCnfRoot(cnfVar, processed, pending);
    }
}

/*----------------------------------------------------------------------------*/
void Expression::toCnf(cnf::CnfExpression& cnfEx, vector<cnf::Variable*>& cnfVar) {

    /* first step: create all the cnf variables */
    creatCnfVariables(cnfEx, cnfVar);

    /* list of terms already processed */
    /* first : negative polarity, second : positive polarity */
    vector<pair<bool, bool>> processed(_operations.size());

    stack<pair<Operation*, bool>> pending;

    /* transform the first term */
    topToCnf(cnfVar, processed, pending);

    while(!pending.empty()) {

        auto inc = pending.top();
        pending.pop();
        if(inc.first->type() == Term::Type::Operation) {
            Operation* op = dynamic_cast<Operation*>(inc.first);
            op->toCnf(inc.second, cnfVar, processed, pending);
        }

    }

    /* display match between exp var and cnf var */
    // cout << endl << endl;
    // for(unsigned int i = 0; i < _terms.size(); i++) {
    //     if(cnfVar.at(i) != nullptr) {
    //         cout << "ex var " << _terms.at(i)->toString() << " -> cnf " << cnfVar.at(i)->toString() << endl;
    //     }
    // }
    // cout << endl << endl;
    //
    // cout << "nbVar " << cnfEx.nVar() << endl;

}

/*----------------------------------------------------------------------------*/
Expression::~Expression() {
    for(auto term : _terms) {
        if(term != nullptr) {
            delete term;
        }
    }
}
