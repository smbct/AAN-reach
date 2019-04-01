/*!
 * \file ANParser.hpp
 * \brief ANParser class definition (for parsing Automata Network)
 * \author S.B
 * \date 13/03/2018
 */

#ifndef AN_PARSER_HPP
#define AN_PARSER_HPP

#include <string>

class AN;

 /*!
  * \class ANParser
  * \brief Parser functions to load an automata network
  */
class ANParser {

    /* public methods */
    public:

        /*!
         * \brief Constructor
         * \param content the automate network file
         * \param an the automata network data structure
         */
        ANParser(std::string content, AN& an);

        /*!
         * \brief parse the "an" file and setup the an data structure
         */
        void parse();

    /* private methods */
    private:

        /*!
         * \brief look for an initial context
         */
        void loadInit();

        /*!
         * \brief load automata from the file content
         */
        void loadAutomata();

        /*!
         * \brief load transitions from the file content
         */
         void loadTransitions();

        /*!
         * \brief remove comments of the content
         */
        void removeComments();

        /*!
         * \brief extract automaton data
         */
        void extractAutomaton(std::string line);

        /*!
         * \brief read a transition from a line
         */
        void extractTransition(std::string line);

    /* private attributes */
    private:

        std::string _content;
        AN& _an;

};


#endif /* AN_PARSER_HPP */
