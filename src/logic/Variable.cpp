/*!
 * \file Variable.cpp
 * \brief Variable class implementation
 * \author S.B
 * \date 20/03/2018
 */

#include "Variable.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
Variable::Variable(Expression& ex) : Term(ex) {
    _type = Term::Variable;
    auto inds = _ex.addVariable(this);
    _ind = inds.first;
    _varInd = inds.second;
}

/*----------------------------------------------------------------------------*/
string Variable::toString() {
    string res = "x_" + to_string(_ind);
    return res;
}

/*----------------------------------------------------------------------------*/
int Variable::varIndex() {
    return _varInd;
}

/*----------------------------------------------------------------------------*/
void Variable::setVarIndex(int varIndex) {
    _varInd = varIndex;
}
