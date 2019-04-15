/*!
 * \file Framework.hpp
 * \brief class Framework to excute the solving procedure
 * \author S.B
 * \date 26/10/2018
 */

#ifndef FRAMEWORK_HPP
#define FRAMEWORK_HPP

/*!
 * \class Framework
 * \brief manage the solution process with handling the parameters
 */
class Framework {

  public:

    /*
     * \brief get the unique parameter instance
     * \return the parameter instance
     */
    static Framework& getFramework();

    /*
     * \brief copy constructor: deleted
     */
    Framework(Framework const&) = delete;

    /*
     * \brief affectation constructor: deleted
     */
    void operator=(Framework const&) = delete;

    /*
     * \brief execute the solver
     */
    void exec();

  private:

    /*
     * \brief constructor
     */
    Framework();

    /*!
     * \brief solve a rechability problem
     */
    void reachability();

    /*!
     * \brief try to compute a bound through k-induction
     */
    void k_induction();

};

#endif
