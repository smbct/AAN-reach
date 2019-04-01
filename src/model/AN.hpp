/*!
 * \file AN.hpp
 * \brief AN class definition (for Automata Network)
 * \author S.B
 * \date 13/03/2018
 */

#ifndef AN_HPP
#define AN_HPP

#include <string>
#include <vector>
#include <map>

/*!
 * \brief Context: list of activated local states, -1 if the state is not specified
 */
typedef std::vector<int> Context;

/*!
 * \brief StrContext: context : local state name for each automaton if specified
 */
typedef std::map<std::string, std::string> StrContext;


/*!
 * \class Transition
 * \brief Data stucture for transitions of automatas
 */
struct Transition {

    int origin;
    int target;
    std::vector<std::pair<int, int>> conditions;
    std::vector<std::pair<int, int>> synchro;

    /*!
     * \brief display the transition in a string
     * \return the string
     */
    std::string toString();

};

/*!
 * \class Automaton
 * \brief Data stucture for automatas of an automata network
 */
struct Automaton {
    std::string name;
    int nState;
    std::vector<std::string> stateName;
    std::map<std::string, int> stateIndex;
    std::vector<Transition> transitions;
};

/*
 * \class LocalState
 * \brief just a simple structure to store automaton local state
 */
struct LocalState {

  /*
   * \brief constructor
   * \param pAut automaton index
   * \param pState state index
   */
  LocalState(int pAut, int pState): automaton(pAut), state(pState) { }

  /*
   * \brief default constructor
   */
  LocalState(): automaton(-1), state(-1) { }

  bool operator==(LocalState& other) {
    return automaton == other.automaton && state == other.state;
  }

  int automaton;
  int state;
};

/*!
 * \class AN
 * \brief Data structure to manage an Automata Network
 */
class AN {

    /* public methods */
    public:

        /*!
         * \brief Default constructor
         * \param filname name of the "an" file
         */
        AN(std::string filename);

        /*!
         * \brief create a void automata network
         */
        AN();

        /*!
         * \brief create a new automaton
         * \param name name of the automaton
         * \return the automaton data strcture
         */
        Automaton& createAutomaton(std::string name);

        /*!
         * \brief access an automaton
         * \param name name of the automaton
         * \return the automaton data structure
         */
        Automaton& getAutomaton(std::string name);

        /*!
         * \brief access an automaton
         * \param index index of the automaton
         * \return the automaton data structure
         */
        Automaton& getAutomaton(int index);

        /*!
         * \brief gives the index of an automaton
         * \param name name of the automaton
         * \return the index of the automaton, -1 if there is no corresponding automaton
         */
        int getAutomatonIndex(std::string name);

        /*!
         * \brief number of automata
         * \return the number of automatas
         */
        int nAutomata();

        /*
         * \brief number of local states in the model
         * \return the number of local states
         */
        int nLocalStates();

        /*
         * \brief number of transitions of the model
         * \return the number of transitions
         */
        int nTransitions();

        /*!
         * \brief gives the initial context of the automata
         * \return the context
         */
        Context& initialContext();

        /*!
         * \brief set the initial state of an automaton
         * \param autInd index of tha automaton
         * \param stateInd index of the state
         */
        void setInitialState(int autInd, int stateInd);

        /*!
         * \brief summary of the an as a string
         * \return the resulting string
         */
        std::string toString();

        /*!
         * \brief convert a string context into a context
         * \param strCtx the string context
         * \param ctx the context
         */
        void strContextToContext(StrContext& strCtx, Context& ctx);

    /* private methods */
    private:

        /*!
         * \brief load the network from an "an" file
         */
        void parse();

    /* private attributes */
    private:

        std::string _filename;
        int _nAut; /* number of automata */
        std::map<std::string, int> _autIndex; /* index of the automatas */
        std::vector<Automaton> _aut; /* liste of the automatas */

        Context _initContext; /* initial context */

};

#endif // AN_HPP
