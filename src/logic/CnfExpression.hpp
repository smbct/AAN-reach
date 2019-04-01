/*!
 * \file CnfExpression.hpp
 * \brief CNF Expression classes definition (representing a CNF logical formula)
 * \author S.B
 * \date 21/03/2018
 */

#ifndef CNF_EXPRESSION_HPP
#define CNF_EXPRESSION_HPP

#include <vector>
#include <string>

namespace cnf {

class Clause;
class Variable;

/*!
 * \class CnfExpression
 * \brief cnf logical formula
 */
class CnfExpression {

    public:

        /*!
         * \brief constructor, create an empty expression
         */
        CnfExpression();

        /*!
         * \brief constructor, load from a file
         * \param fileName the name of the dimac file
         */
        CnfExpression(std::string fileName);

        /*!
         * \brief create a new variable for the expression
         * \return the index of the variable
         */
        int addVariable(Variable* var);

        /*!
         * \brief add a clause into the formula
         * \param clause the clause
         */
        void addClause(Clause* clause);

        /*!
         * \brief number of variable in the expression
         * \return the number of variable
         */
        int nVar();

        /*!
         * \brief number of clauses in the expression
         * \return the number of clauses
         */
        int nClause();

        /*!
         * \brief convert to a string
         * \return the string
         */
        std::string toString();

        /*!
         * \brief export to the Dimacs format
         * \param fileName name of the file
         */
        void exportDimacs(std::string fileName);

        /*!
         * \brief destructor
         */
        ~CnfExpression();

    private:

        std::vector<Clause*> _clauses;
        std::vector<Variable*> _variables;

};

/*!
 * \class Variable
 * \brief cnf variable
 */
class Variable {

    public:

        /*!
         * \brief constructor
         * \param ex the expression of the variable
         */
        Variable(CnfExpression& ex);

        /*!
         * \brief convert to a string
         * \return the string
         */
        std::string toString();

        /*!
         * \brief get the expression of the variable
         * \return the expression
         */
        CnfExpression& expression();

        /*!
         * \brief get the index of the variable
         * \return the index
         */
        int index();

    private:
        int _ind;
        CnfExpression& _ex;
};


/*!
 * \class Literal
 * \brief Literal of a clause, can be a variable or its negation
 */
class Literal {

    public:

        /*!
         * \brief constructor
         * \param var the variable of the literal
         * \param pos false if there is a negation
         */
        Literal(Variable* var, bool pos = true);

        /*!
         * \brief variable of the literal
         * \return the variable
         */
        Variable& variable();

        /*!
         * \brief indicate if the literal is positive or not
         * \return true iff the literal si positive
         */
        bool positive();

        /*!
         * \brief convert to a string
         * \return the string
         */
        std::string toString();

    private:

        Variable& _var;
        bool _pos; /* positive or negative literal */

};

class Clause {

    public:

        /*!
         * \brief constructor
         * \param lit the literals of the clause
         */
        Clause(std::vector<Literal> lit);

        /*!
         * \brief convert to a string
         * \return the string
         */
        std::string toString();

        /*!
         * \brief get a reference of the literals
         * \return the literals
         */
        std::vector<Literal>& literals();

    private:
        std::vector<Literal> _lit; /* list of literals in the clause */

};







}


#endif /* CNF_EXPRESSION_HPP */
