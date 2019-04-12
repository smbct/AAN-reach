/*!
 * \file Parameters.hpp
 * \brief class Parameters and functions for command line usage
 * \author S.B
 * \date 25/10/2018
 */

#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include "../model/AN.hpp"

/*!
 * \brief Parameters class for the command line usage
 */
class Parameters {

  public:
    enum Encoding {undefined, ASP, SAT};
    enum Solver {minisat, glucose, maplecomsps, ccanr};

  public:

    /*
     * \brief get the unique parameter instance
     * \return the parameter instance
     */
    static Parameters& getParameters();

    /*
     * \brief copy constructor: deleted
     */
    Parameters(Parameters const&) = delete;

    /*
     * \brief affectation constructor: deleted
     */
    void operator=(Parameters const&) = delete;

    /*
     * \brief display the parameters
     */
    void display();

    /*
     * \brief load the parameters from the command line arguments
     * \param argc number of arguments
     * \param argv list of arguments
     */
    void loadFromCmd(int argc, char** argv);

    /*
     * \brief show the help
     */
    void showHelp();

  public:

    Encoding encoding; /* the encoding: SAT or ASP */
    Solver solver; /* the SAT solver chosen */
    std::string model; /* path of the automata network file */
    StrContext initialState; /* the initial state of the rechability problem */
    StrContext goal; /* the goal of the rechability problem */
    int debugLevel; /* level for debuging information */
    bool help; /* show the help */
    int bound; /* bound of the reachability sequence */

  private:

    /*
     * \brief constructor
     */
    Parameters();

    /*
     * \brief extract a context from the argument
     * \param token the argument
     * \param ctx the context modified
     */
    static void extractContext(std::string token, StrContext& ctx);


};



#endif
