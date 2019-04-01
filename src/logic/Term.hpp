/*!
 * \file Term.hpp
 * \brief Term class and sub classes definition (representing a term in a logical formula)
 * \author S.B
 * \date 14/03/2018
 */

#ifndef TERM_HPP
#define TERM_HPP

#include <string>
#include <stack>
#include <vector>

#include "Expression.hpp"


/*!
 * \class Term
 * \brief term of a logical expression
 */
class Term {

    /* public enumeration */
    public:

        enum Type {
            Undefined,
            Variable,
            Operation
        };


    /* public functions */
    public:

        /*!
         * \brief Constructor
         * \param ex the logical expression
         */
        Term(Expression& ex);

        /*!
         * \brief get the type of the term
         * \return the type
         */
        Type type();

        /*!
         * \brief set the index of the term
         * \param index the new index
         */
        void setIndex(int index);

        /*!
         * \brief get the index of the term
         * \return the new index
         */
        int index();

        /*!
         * \brief return the term as a string
         */
        virtual std::string toString() = 0;

        /*!
         * \brief return the expression the term belongs
         */
        Expression& expression();

        /*!
         * \brief create a literal for a cnf variable (used to manage the not case (no cnf variable for not operation))
         * \param cnfVar the cnf variables
         * \param polarity the polarity of the variable (positive or negative)
         */
        virtual cnf::Literal createCnfLiteral(std::vector<cnf::Variable*>& cnfVar, bool polarity);

        /*!
         * \brief default destructor
         */
        virtual ~Term();

    /* protected attributes */
    protected:

        Expression& _ex; /* expression of the term */
        Type _type; /* term type */
        int _ind; /* term index in the expression */


};

#endif /* TERM_HPP */
