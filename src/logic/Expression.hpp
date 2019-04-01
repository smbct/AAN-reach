/*!
 * \file Expression.hpp
 * \brief Expression class definition (representing a logical formula)
 * \author S.B
 * \date 14/03/2018
 */

#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <stack>

#include "CnfExpression.hpp"

class Term;
class Variable;
class Operation;
class OrOp;
class ImplyOp;

/*!
 * \class Expression
 * \brief Represent a logical expression as a tree
 */
class Expression {

    /* public methods */
    public:

        /*!
         * \brief Constructor
         */
        Expression();

        /*!
         * \brief set main top term of the expression
         * \param term the main term
         */
        void setMainTerm(Term* term);

        /*!
         * \brief return the number of variable in the expression
         * \return the number of variable
         */
        int nVar();

        /*!
         * \brief add a new variable and return its index
         * \param var the variable to add
         * \return term index and variable index of the new variable
         */
        std::pair<int,int> addVariable(Variable* var);

        /*!
         * \brief add a new operation and return its index
         * \param op the operation to add
         * \return term index and operation index of the new operation
         */
        std::pair<int,int> addOperation(Operation* op);

        /*!
         * \brief get the variables of the expression
         * \return the variables vector
         */
        std::vector<Variable*>& variables();

        /*!
         * \brief get the operations of the expression
         * \return the operations vector
         */
        std::vector<Operation*>& operations();

        /*!
         * \brief get the terms of an expression
         * \return a reference toward the vector terms
         */
        std::vector<Term*>& terms();

        /*!
         * \brief return the expression as a string
         * \return the string
         */
        std::string toString();

        /*!
         * \brief reduce the ewpression : translate the "->" into "or"
         */
        void reduce();

        /*!
         * \brief convert the formula to a cnf formula
         * \param ex the new cnf expression
         * \param cnfVar list of all cnf variable (corresponding to the ex term id)
         */
        void toCnf(cnf::CnfExpression& cnfEx, std::vector<cnf::Variable*>& cnfVar);

        /*!
         * \brief destructor, destruct all the variables and operations
         */
        ~Expression();

    /* private methods */
    private:

        /*!
         * \brief add a new term
         * \param term the new term
         * \return the index of the new term
         */
        int addTerm(Term* term);

        /*!
         * \brief transform implies into or
         */
        void removeImply();

        /*!
         * \brief transform an imply into an or
         * \param or the imply operation
         * \return the new or operation
         */
        OrOp* transformImply(ImplyOp* imply);

        /*!
         * \brief update the operations list, remove deleted operations
         */
        void updateOperationsPool();

        /*!
         * \brief create the cnf variables for the transformation
         * \param cnfEx the cnf expression
         * \param cnfVar the list of the cnf variables
         */
        void creatCnfVariables(cnf::CnfExpression& cnfEx, std::vector<cnf::Variable*>& cnfVar);

        /*!
         * \brief convert the top term to a cnf expression
         * \param pending the pending list
         * \cnfVar the cnf variables
         */
        void topToCnf(std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);

    /* protected attributes */
    protected:

        Term* _term; /* top term of the expression */

        std::vector<Variable*> _variables; /* list of all variables */

        std::vector<Operation*> _operations; /* list of all operations */

        std::vector<Term*> _terms; /* list of all terms */
};

#endif
