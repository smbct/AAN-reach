/*!
 * \file Encoding.hpp
 * \brief classes and functions to encode Automata network analysis into SAT
 * \author S.B
 * \date 27/03/2018
 */

#ifndef ENCODING_HPP
#define ENCODING_HPP

#include "../model/AN.hpp"

#include "../logic/Expression.hpp"
#include "../logic/Variable.hpp"
#include "../logic/Operation.hpp"

typedef std::vector<std::vector<Variable*>> StateVar;

/*!
 * \class Encoding
 * \brief functions to encode AN analysis into SAT problems
 */
class Encoding {

    public:

        /*!
         * \brief constructor
         * \param an the automata network
         */
        Encoding(AN& an);

        /*!
         * \brief encode the reachability problem
         * \param initCtx the initial context
         * \param finalCtx the final context
         * \param length the length of the path encoded
         * \return true if the final context is reachable from the initial context
         */
        bool reachability(Context& initCtx, Context& finalCtx, int length);

        /*!
         * \brief apply the k-induction techniques to look for a bound
         * \param finalCtx the final context
         * \param length the length of the unconstrained path encoded
         * \return true if the final context is reachable from the initial context
         */
        bool k_induction(Context& finalCtx, int length);

        /*!
         * \brief change the verbose variable
         * \param activate true if verbose will be activated
         */
        void setVerbose(bool activate);

        /*!
         * \brief change the variable for solution extraction
         * \param extract the new value
         */
        void setExtractSolution(bool extract);

    private: /* private methods */

        /*!
         * \brief create the logical variables of a path
         * \param length length of the path
         */
        void createPathVariables(int length);

        /*!
         * \brief add a context constraint for a specific global state
         * \param stateVar variables of the state
         * \param context the context
         */
        void setContext(StateVar& stateVar, Context& context);

        /*!
         * \brief force a global state to be activated (at least one local state is activated)
         * \param stateVar the variables of the global state
         */
        void activateState(StateVar& stateVar);

        /*!
         * \brief force a local state to be disabled
         * \param stateVar the variables of the global state
         * \param autInd the automata index
         * \param stateInd the state index
         */
        void disableState(StateVar& stateVar, int autInd, int stateInd);

        /*!
         * \brief avoid conflict between the local states
         * \param stateVar variables of the local state
         */
        void avoidConflict(StateVar& stateVar);

        /*!
         * \brief create a transition between two global states
         * \param sv1 the first global state variables
         * \param sv2 the second global state variables
         * \param k the index of the step in the BMC path
         */
        void createTransition(StateVar& sv1, StateVar& sv2, int k);

        /*!
         * \brief create an activation rule for a local state during a transition
         * \param sv1 the previous global state
         * \param sv2 the global state of the rule
         * \param autInd the index of the automata of the rule
         * \param stateInd the index of the state of the rule
         * \param k index of the step in the BMC path
         */
        void activationRule(StateVar& sv1, StateVar& sv2, int autInd, int stateInd, int k);

        /*!
         * \brief force two global states to be different
         * \param sv1 the first global state
         * \param sv2 the second global state
         */
        void statesDiff(StateVar& sv1, StateVar& sv2);

        /*!
         * \brief add a alldiff constraint to a path
         * \param variables the logical variables of the path
         */
         void allDiff(std::vector<StateVar>& variables);

        /*!
         * \brief extract the solution returned by the SAT solver
         * \param stateVar list of the state variables of the path
         * \param cnfVar list of the cnf Variables
         * \param length length of the path
         * \return true of the SAT instance was satisfiable
         */
        bool extractSolution(std::vector<StateVar>& stateVar, std::vector<cnf::Variable*>& cnfVar, int length);

        /*!
         * \brief get a noChange node
         * \param k the step in the BMC path
         * \param autInd the index of the automata that is not changed
         * \return the and node for the noChange constraint
         */
        AndOp* noChange(int k, int autInd);

        /*
         * \brief convert a string context to a context
         * \param strCtx the string context
         * \param ctx the context
         */
        void convertContext(StrContext& strCtx, Context& ctx);

    private:

        AN& _an; /* automata network to analyse */

        Expression _ex; /* logical expression */

        std::vector<Term*> _termList; /* list of all constraints for the logical expression */

        std::vector<StateVar> _variables; /* BMC path variables */

        std::vector<std::vector<AndOp*>> _noChange; /* no change loc state node */

        bool _verbose; /* display the final path */

        bool _extractSolution; /* extract the solution */

};

#endif
