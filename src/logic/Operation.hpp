/*!
 * \file Operation.hpp
 * \brief Operation class definition (representing an operation in a logical formula)
 * \author S.B
 * \date 20/03/2018
 */

#ifndef OPERATION_HPP
#define OPERATION_HPP

#include "Term.hpp"

#include <vector>

/*!
 * \class Operation
 * \brief Operation representing a logical operation
 */
class Operation : public Term {

    public: /* public enum */

        enum Operator {
            None,
            Not,
            Or,
            And,
            Imply,
            Equivalent
        };

    public:

        /*!
         * \brief constructor
         * \param ex the expression the operation belongs
         */
        Operation(Expression& ex);

        /*!
         * \brief get the terms of the operation
         * \return a reference to the terms vector
         */
        std::vector<Term*>& terms();

        /*!
         * \brief get the operator type of the operation
         * \return the operator
         */
        Operator operatorType();

        /*!
         * \brief get the expression index of the operation
         * \return the index
         */
        int opIndex();

        /*!
         * \brief set the index of the operation
         * \param index the new index
         */
        void setOpIndex(int opIndex);

        /*!
         * \brief reduce the sub expression
         * \param pending the pending terms
         * \return true if the expression has been reduced
         */
        virtual bool reduce(std::stack<Term*>& pending);

        /*!
         * \brief convert the operation into a cnf formula
         * \param polarity polarity of the operation
         * \param cnfVar list of the cnf variables
         * \param processed list of terms already processed
         * \param cnfEx the cnf expression
         * \param pending the remaining terms to transform
         */
        virtual void toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending) = 0;

        /*!
         * \brief convert the operation into a cnf formula as the root of the formula
         * \param cnfVar list of the cnf variables
         * \param processed list of terms already processed
         * \param cnfEx the cnf expression
         * \param pending the remaining terms to transform
         */
        virtual void toCnfRoot(std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending) = 0;

        /*!
         * \brief give de number of reference
         * \return the number of ref
         */
        short int nRef();

    /* protected methods */
    protected:

        /*!
         * \brief update the pending list of operations to transform into cnf
         * \param processed list of operations already processed
         * \param pending list of remaining operations
         * \param pol : polarity : 0: both, 1: negative, 2: positive
         */
        void updatePendingCnf(std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending, int pol);

        /*!
         * \brief update the pending list of operations to transform into cnf, add the term term
         * \param processed list of operations already processed
         * \param pending list of remaining operations
         * \param term the term to be added
         * \param pol : polarity : 0: both, 1: negative, 2: positive
         */
        void updatePendingCnf(std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending, Term* term, int pol);

        /*!
         * \brief add a reference to this operation
         */
        void increaseRef();

    /* private methods */
    private:

        /*!
         * \brief update the number of reference of the sub operations
         */
        void updateRef();

    protected:
        Operator _op;
        int _opInd; /* operation index */
        std::vector<Term*> _terms; /* list of operands */
        short int _nRef; /* number of time the operation is used */
};

/*!
 * \class OrOperation
 * \brief representing a logical or operation
 */
class OrOp : public Operation {

    public:

        /*!
         * \brief constructor
         * \param terms the list of terms in the or operation
         */
        OrOp(std::vector<Term*> terms);

        virtual std::string toString();

        virtual bool reduce(std::stack<Term*>& pending);

        virtual void toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);

        virtual void toCnfRoot(std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);

    protected:



};

/*!
 * \class AndOp
 * \brief representing a logical and operation
 */
class AndOp : public Operation {

    public:

        /*!
         * \brief constructor
         * \param terms the list of terms in the and operation
         */
        AndOp(std::vector<Term*> terms);

        virtual std::string toString();

        virtual void toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);

        virtual void toCnfRoot(std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);


    protected:

};

/*!
 * \class NotOp
 * \brief representing a logical not operation
 */
class NotOp : public Operation {

    public:
        /*!
         * \brief constructor
         * \param term the term not is applied apply
         */
        NotOp(Term* term);

        virtual std::string toString();

        virtual void toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);

        virtual cnf::Literal createCnfLiteral(std::vector<cnf::Variable*>& cnfVar, bool polarity);

        virtual void toCnfRoot(std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);


    protected:

};

/*!
 * \class ImplyOp
 * \brief representing a logical imply operation
 */
class ImplyOp : public Operation {

    public:

        /*!
         * \brief constructor, build the left -> right operation
         * \param left the left term
         * \param right the right term
         */
        ImplyOp(Term* left, Term* right);

        virtual std::string toString();

        virtual void toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);

        virtual void toCnfRoot(std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);


    private:

};

/*!
 * \class EquivalentOp
 * \brief representing a logical equivalent operation
 */
class EquivalentOp : public Operation {

    public:

        /*!
         * \brief constructor, build the left <-> right operation
         * \param left the left term
         * \param right the right term
         */
        EquivalentOp(Term* left, Term* right);

        virtual std::string toString();

        virtual void toCnf(bool polarity, std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);

        virtual void toCnfRoot(std::vector<cnf::Variable*>& cnfVar, std::vector<std::pair<bool, bool>>& processed, std::stack<std::pair<Operation*, bool>>& pending);

    private:

};

#endif /* OPERATION_HPP */
