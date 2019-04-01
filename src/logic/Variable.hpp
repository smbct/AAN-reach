/*!
 * \file Variable.hpp
 * \brief Variable class definition (representing a logical variable)
 * \author S.B
 * \date 20/03/2018
 */

#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "Term.hpp"

/*!
 * \class Variable
 * \brief define a logical variable, linked to an expression
 */
class Variable : public Term {

    public:

        /*!
         * \brief default constructor
         * \param ex the logical expression the variable belongs
         */
        Variable(Expression& ex);

        /*!
         * \brief display the variable as a string
         * \return the string of the variable
         */
        std::string toString();

        /*!
         * \brief get the index of the variable
         * \return the index
         */
        int varIndex();

        /*!
         * \brief set the index of the variable
         * \param varIndex the index of the variable
         */
        void setVarIndex(int varIndex);

    private:

        int _varInd; /* index of the variable in the expression */

};

#endif /* VARIABLE_HPP */
