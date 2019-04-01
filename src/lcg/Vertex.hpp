/*!
 * \file Vertex.hpp
 * \brief class to represent a vertex of the local causality graph
 * \author S.B
 * \date 18/04/2018
 */

#ifndef LCG_VERTEX_HPP
#define LCG_VERTEX_HPP

#include <list>
#include <vector>
#include <string>

namespace LCG {

enum VertexType { UndefinedVert, LocalStateVert, SolutionVert, ObjectiveVert, TransitionVert };

/* forward declaration */
class Graph;

/*!
 * \class Vertex
 * \brief General vertex of the local causality graph
 */
class Vertex {

    public:

        /*!
         * \brief constructor
         * \param pgraph the local causality graph of the vertex
         */
        Vertex(Graph* pgraph);

        /*!
         * \brief display the vertex as a string
         * \return the string
         */
        virtual std::string toString() = 0;

        /*!
         * \brief compute a bound on the number of transitions needed
         * \param statut statut of the vertices
         * \param bound bound computed
         */
        virtual void computeBound(std::vector<int>& statut, std::vector<int>& bound) = 0;

        /*!
         * \brief get the successors of the vertex
         */
        void getSuccessors(std::list<Vertex*>& suclist);

        /*!
         * \brief find if there is a path to a transition, not containing a specific vertex
         * \param forbidden the vertex which can't be in the path
         * \return true iff there is a path
         */
        bool existsPath(Vertex* forbidden);

        /*!
         * \brief get the predecessors of the vertex
         * \param predList list of the predecessors
         */
        void getPredecessord(std::list<Vertex*>& predList);


        /*!
         * \brief indicate if vert is an (undirect) successor of the vertex
         * \return true iff vert is a successor
         */
        bool isSuccessor(Vertex* vert);

        /*!
         * \brief destructor
         */
        virtual ~Vertex();

    public:

        VertexType type; /* type of the LCG vertex */
        Graph* graph; /* pointer toward the local causality graph */
        int index; /* index of the vertex in the LCG Graph */

        std::list<Vertex*> suc; /* successors of the vertex in the graph */
        std::list<Vertex*> pred; /* predecessors of the vertex */

};

/*!
 * \class VLocalState
 * \brief Vertex representing a local state in the LCG
 */
class VLocalState: public Vertex {

    public:

        /*!
         * \brief constructor
         * \param pgraph the local causality graph of the vertex
         * \param pautInd automaton index
         * \param pstateInd local state index
         */
        VLocalState(Graph* pgraph, int pautInd, int pstateInd);

        virtual void computeBound(std::vector<int>& statut, std::vector<int>& bound);

        virtual std::string toString();

    public:
        int autInd; /* index of the automaton in the automata network */
        int stateInd; /* index of the automaton's state */

};

/*!
 * \class VSolution
 * \brief Solution vertex in the LCG
 */
class VSolution: public Vertex {

    public:

        /*!
         * \brief constructor
         * \param pgraph the local causality graph of the vertex
         * \param autInd index of the automaton
         * \param trSequence sequence of transitions indexes
         */
        VSolution(Graph* pgraph, int autInd, std::list<int>& trSequence);

        /*!
         * \brief constructor, build an empty solution (for trivial objectives)
         * \param pgraph the local causality graph
         */
        VSolution(Graph* pgraph);

        virtual void computeBound(std::vector<int>& statut, std::vector<int>& bound);

        virtual std::string toString();

    private:

        /*!
         * \brief update the transitions vertices
         * \param autInd the automaton index
         * \param trSequence the sequence of transition indexes
         */
        void updateVTransitions(int autInd, std::list<int>& trSequence);

};

/*!
 * \class VTransition
 * \brief A transition in the local causality graph
 */
class VTransition: public Vertex {

    public:

        /*!
         * \brief constructor
         * \param pgraph the local causality graph of the vertex
         * \param pautInd automaton index
         * \param ptrInd transition index
         */
        VTransition(Graph* pgraph, int pautInd, int ptrInd);

        virtual void computeBound(std::vector<int>& statut, std::vector<int>& bound);

        virtual std::string toString();

    /* private methods */
    private:

        /*!
         * \brief add the local states required to the graph
         */
        void addVLocalStates();

    public:
        int autInd; /* index of the automaton in the automata network */
        int trInd; /* index of the transition in the automata network */

};

}

#endif /* LCG_VERTEX_HPP */
