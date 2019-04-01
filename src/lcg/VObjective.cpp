/*!
 * \file VObjective.hpp
 * \brief implementation of the VObjective class
 * \author S.B
 * \date 19/04/2018
 */

#include "VObjective.hpp"
#include "Graph.hpp"

#include <iostream>

using namespace LCG;
using namespace std;

/*----------------------------------------------------------------------------*/
VObjective::VObjective(Graph* pgraph, int pautInd, int poriginState, int ptargetState):
Vertex(pgraph),
autInd(pautInd),
originState(poriginState),
targetState(ptargetState)
{
    type = VertexType::ObjectiveVert;
    graph->addVertex(this);

    solve();
}

/*----------------------------------------------------------------------------*/
void VObjective::solve() {

    AN& an = graph->automataNewtork();
    Automaton& aut = an.getAutomaton(autInd);

    /* list of visited local states */
    vector<bool> visited(aut.nState, false);
    visited.at(originState) = true;

    /* sequence of transitions to reach the local state */
    list<int> sequence;

    /* trivial objective : only the empty solution */
    if(originState == targetState) {
        VSolution* vsol = new VSolution(graph);
        suc.push_back(vsol);
        vsol->pred.push_back(this);
    } else { /* find the scenario associated to the solutions */
        recursiveSolve(sequence, visited);
    }

}

/*----------------------------------------------------------------------------*/
void VObjective::recursiveSolve(list<int>& sequence, vector<bool>& visited) {

    Automaton& aut = graph->automataNewtork().getAutomaton(autInd);

    /* base case */
    if(!sequence.empty() && aut.transitions.at(sequence.back()).target == targetState) {
        VSolution* vsol = new VSolution(graph, autInd, sequence);
        suc.push_back(vsol);
        vsol->pred.push_back(this);
    } else {

        /* recursive call for every possible transition */
        for(unsigned int trInd = 0; trInd < aut.transitions.size(); trInd ++) {

            Transition& tr = aut.transitions.at(trInd);

            bool valid = false;
            if(sequence.empty() && tr.origin == originState) {
                valid = true;
            } else if(!sequence.empty() && tr.origin == aut.transitions.at(sequence.back()).target && !visited.at(tr.target)) {
                valid = true;
            }

            /* recursive case */
            if(valid) {
                visited.at(tr.target) = true;
                sequence.push_back(trInd);
                recursiveSolve(sequence, visited);
                sequence.pop_back();
                visited.at(tr.target) = false;
            }

        }

    }

}

/*----------------------------------------------------------------------------*/
void VObjective::computeBound(vector<int>& statut, vector<int>& bound) {

    /* objective: max of the solutions bouns */

    bound.at(index) = -1;

    for(Vertex* vert : suc) {
        if(bound.at(vert->index) > bound.at(index)) {
            bound.at(index) = bound.at(vert->index);
        }
    }

}

/*----------------------------------------------------------------------------*/
string VObjective::toString() {
    string res = "";
    Automaton& aut = graph->automataNewtork().getAutomaton(autInd);
    res += aut.name + "_" + aut.stateName.at(originState) + " ~> " + aut.name + "_" + aut.stateName.at(targetState) + " (" + to_string(index) + ")";
    return res;
}
