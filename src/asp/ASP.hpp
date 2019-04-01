/*!
 * \file ASP.hpp
 * \brief classes and functions to encode Automata network rechability analysis into ASP from Emna's PhD manuscript
 * \author S.B
 * \date 24/10/2018
 */

#ifndef ASP_HPP
#define ASP_HPP

#include "../model/AN.hpp"


/*!
 * \class ASP
 * \brief rehcability encoder into ASP program
 */
class ASP {

  public:

    /*!
     * \brief constructor
     * \param an the automata network
     */
    ASP(AN& an);

    /*!
     * \brief encode the reachability problem
     * \param initContext the initial context
     * \param finalContext the final context
     * \param length the length of the path encoded
     * \return true if the final context is reachable from the initial context
     */
    bool reachability(Context& initContext, Context& finalContext, int length);

  private:

    /*!
    * \brief create the ASP encoding of the model information
    * \return the partial ASP program
    */
    std::string aspNet();

    /*!
     * \brief create the execution path
     * \param length the length of the execution sequence
     * \return the ASP program
     */
    std::string createPath(int length);

  private:
    AN& _an;


};

#endif
