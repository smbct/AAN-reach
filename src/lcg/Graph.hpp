/*!
 * \file Graph.hpp
 * \brief classes and functions to define the local causality graph of a reachability problem
 * \author S.B
 * \date 03/04/2018
 */

#ifndef LCG_GRAPH_HPP
#define LCG_GRAPH_HPP

#include "../model/AN.hpp"
#include "Vertex.hpp"
#include "VObjective.hpp"

#include <vector>

namespace LCG {

/*
 * \class Graph
 * \brief Local causality graph of a specific accessibility problem, gives informations on the solution
 */
class Graph {

    public:

        /*!
         * \brief contructor
         * \param an the automata network of the problem
         * \param initContext the initial context of the problem
         * \param goal state the local state to reach
         */
        Graph(AN& an, Context& initContext, LocalState& goalState);

        /*!
         * \brief build the local causality graph
         */
        void build();

        /*!
         * \brief compute a bound on the shortest path for the accessibility problem
         * \return the bound
         */
        int computeBound();

        /*!
         * \brief compute the bound for a specific vertex in the LCG
         * \pre there is no cycles in the sub graph of the vertex
         * \param vert the vertex
         * \param statut the statut of the vertices
         * \param bounds the bounds
         */
        void computeBound(Vertex* vert, std::vector<int>& statut, std::vector<int>& bound);

        /*!
         * \brief add a vertex to the local causality graph
         */
        void addVertex(Vertex* vertex);

        /*!
         * \brief get a transition vertex, created if necessary
         * \param autInd the automaton index
         * \param trInd the transition index
         * \return the transition vertex
         */
        VTransition* getVTransition(int autInd, int trInd);

        /*!
         * \brief get a local state vertex from the graph, created if necessary
         * \param autInd the automaton index
         * \param staetInd the local state index
         * \return the local state vertex
         */
        VLocalState* getVLocalState(int autInd, int stateInd);

        /*!
         * \brief get the automata network of the graph
         * \return the automata network
         */
        AN& automataNewtork();

        /*!
         * \brief get the reduced automata network
         * \return the reduced automata network
         */
        AN* reducedNetwork();

        /*!
         * \brief get the number of vertices in the graph
         * \return the number of vertices
         */
        int nVertices();

        /*!
         * \brief compute the number of edges in the graph
         */
        int nEdges();

        /*!
         * \brief check if there are cycles in the graph
         * \return true iff there are cycles
         */
        bool checkCycle();

        /*!
         * \brief destructor, free the memory
         */
        ~Graph();

    private:

        /*!
         * \brief allocate the maps
         */
        void setupMaps();

        /*!
         * \brief update a local state of the graph
         * \param vlocalState the local state vertex
         * \return true if a new objective has been added
         */
        bool updateLocalState(VLocalState* vlocalState);

        /*!
         * \brief check for necessary re-centering: if the automaton is move from its objective
         * \return true iff the graph has been modified
         */
        bool re_centering();



    private:

        /* vertices of the local causality graph */
        std::vector<Vertex*> _vertices;

        /* list of vertices by their type, so that it can be easily found */
        std::vector<VLocalState*> _vlocalStates;
        std::vector<VSolution*> _vsolutions;
        std::vector<VObjective*> _vobjectives;
        std::vector<VTransition*> _vtransitions;

        /* map between vertices and network indexes */
        std::vector<std::vector<VLocalState*>> _vlocalStateMap;
        std::vector<std::vector<std::vector<VObjective*>>> _vobjectiveMap;
        std::vector<std::vector<VTransition*>> _vtransitionMap;

        AN& _an; /* automata network of the problem */
        Context& _initContext; /* initial context of the reachability problem */

        LocalState& _goalState; /* goal local state */

        VLocalState* _root; /* "root" of the graph */
};


}

#endif /* LCG_GRAPH_HPP */
