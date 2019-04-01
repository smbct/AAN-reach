/*!
 * \file VObjective.hpp
 * \brief definition of the VObjective class (objective vertex)
 * \author S.B
 * \date 20/04/2018
 */

#ifndef LCG_VOBJECTIVE_HPP
#define LCG_VOBJECTIVE_HPP

#include "Vertex.hpp"

namespace LCG {

    /*!
     * \class VObjective
     * \brief Represent an objective in the local causality graph
     */
    class VObjective: public Vertex {

        public:

            /*!
             * \brief constructor
             * \param pgraph the local causality graph of the vertex
             * \param autInd index of the automaton
             * \param originState index of the origin state
             * \param targetState index of the target state
             */
            VObjective(Graph* pgraph, int pautInd, int poriginState, int ptargetState);

            virtual void computeBound(std::vector<int>& statut, std::vector<int>& bound);

            virtual std::string toString();

        private:

            /*!
             * \brief find all the solutions of the objective, add them to the graph
             */
            void solve();

            /*!
             * \brief recursive solve the objective : find the transition sequence to achieve the objective
             * \param sequence of transition indexes
             * \param visited visited local states
             */
            void recursiveSolve(std::list<int>& sequence, std::vector<bool>& visited);

        public:

            int autInd; /* index of the automata */
            int originState; /* index of the origin state */
            int targetState; /* index of the goal state */

    };


}

#endif /* LCG_VOBJECTIVE_HPP */
