/*!
 * \file Vertex.cpp
 * \brief Implementation of the vertex classses
 * \author S.B
 * \date 18/04/2018
 */

#include "Vertex.hpp"
#include <iostream>
#include <stack>

#include "Graph.hpp"

using namespace std;
using namespace LCG;

/*----------------------------------------------------------------------------*/
Vertex::Vertex(Graph* pgraph):
type(VertexType::UndefinedVert)
{
    this->graph = pgraph;
}

/*----------------------------------------------------------------------------*/
void Vertex::getSuccessors(std::list<Vertex*>& suclist) {

    vector<bool> visited(graph->nVertices(), false);

    stack<Vertex*> pending;

    for(Vertex* sucv : suc) {
        if(!visited.at(sucv->index)) {
            pending.push(sucv);
            suclist.push_back(sucv);
            visited.at(sucv->index) = true;
        }
    }

    while(!pending.empty()) {

        Vertex* top = pending.top();
        pending.pop();

        for(Vertex* sucv : top->suc) {
            if(!visited.at(sucv->index)) {
                pending.push(sucv);
                suclist.push_back(sucv);
                visited.at(sucv->index) = true;
            }
        }
    }

}

/*----------------------------------------------------------------------------*/
void Vertex::getPredecessord(std::list<Vertex*>& predList) {

  vector<bool> visited(graph->nVertices(), false);

  stack<Vertex*> pending;

  for(Vertex* predv : pred) {
      if(!visited.at(predv->index)) {
          pending.push(predv);
          predList.push_back(predv);
          visited.at(predv->index) = true;
      }
  }

  while(!pending.empty()) {

      Vertex* top = pending.top();
      pending.pop();

      for(Vertex* predv : top->pred) {
          if(!visited.at(predv->index)) {
              pending.push(predv);
              predList.push_back(predv);
              visited.at(predv->index) = true;
          }
      }
  }

}

/*----------------------------------------------------------------------------*/
bool Vertex::isSuccessor(Vertex* vert) {

  bool res = false;

  /* TODO : use a search tree ? */
  vector<bool> visited(graph->nVertices(), false);

  stack<Vertex*> pending;

  for(Vertex* sucv : suc) {
      if(!visited.at(sucv->index)) {
          pending.push(sucv);
          visited.at(sucv->index) = true;
      }
  }

  while(!pending.empty() && !res) {

    Vertex* top = pending.top();
    pending.pop();

    if(top == vert) {
      res = true;
    } else {
      for(Vertex* sucv : top->suc) {
        if(!visited.at(sucv->index)) {
          pending.push(sucv);
          visited.at(sucv->index) = true;
        }
      }
    }
  }

  return res;
}

/*----------------------------------------------------------------------------*/
Vertex::~Vertex() {

}


/*----------------------------------------------------------------------------*/
VLocalState::VLocalState(Graph* pgraph, int pautInd, int pstateInd):
Vertex(pgraph),
autInd(pautInd),
stateInd(pstateInd)
{
    type = VertexType::LocalStateVert;
    graph->addVertex(this);
}

/*----------------------------------------------------------------------------*/
void VLocalState::computeBound(vector<int>& statut, vector<int>& bound) {

    bound.at(index) = -1;

    /* local state : max of the objective bounds */
    for(Vertex* vert : suc) {
        if(bound.at(vert->index) > bound.at(index)) {
            bound.at(index) = bound.at(vert->index);
        }
    }

}

/*----------------------------------------------------------------------------*/
string VLocalState::toString() {
    string res = "";
    Automaton& aut = graph->automataNewtork().getAutomaton(autInd);
    res += aut.name + "_" + aut.stateName.at(stateInd) + " (" + to_string(index) + ")";
    return res;
}



/*----------------------------------------------------------------------------*/
VTransition::VTransition(Graph* pgraph, int pautInd, int ptrInd):
Vertex(pgraph),
autInd(pautInd),
trInd(ptrInd)
{
    type = VertexType::TransitionVert;
    graph->addVertex(this);

    /* add the local states to the graph */
    addVLocalStates();
}

/*----------------------------------------------------------------------------*/
void VTransition::addVLocalStates() {

    Automaton& aut = graph->automataNewtork().getAutomaton(autInd);

    /* find all the local states of the transtion */
    Transition& tr = aut.transitions.at(trInd);

    for(auto cond : tr.conditions) {
        VLocalState* vlocalState = graph->getVLocalState(cond.first, cond.second);
        suc.push_back(vlocalState);
        vlocalState->pred.push_back(this);
    }

}

/*----------------------------------------------------------------------------*/
void VTransition::computeBound(vector<int>& statut, vector<int>& bound) {
    /* transition : sum of local states dependences + 1 */
    bool error = false;
    bound.at(index) = 1;
    for(Vertex* vert : suc) {
        if(bound.at(vert->index) >= 0) {
            bound.at(index) += bound.at(vert->index);
        } else {
            error = true;
        }
    }

    if(error) {
        bound.at(index) = -1;
    }
}

/*----------------------------------------------------------------------------*/
string VTransition::toString() {
    string res = "";
    Automaton& aut = graph->automataNewtork().getAutomaton(autInd);
    Transition& tr = aut.transitions.at(trInd);
    res += aut.name + "_" + aut.stateName.at(tr.origin) + " -> " + aut.name + "_" + aut.stateName.at(tr.target);
    res += " {";
    unsigned int i = 0;
    for(auto cond : tr.conditions) {
        res += graph->automataNewtork().getAutomaton(cond.first).name + "_" + graph->automataNewtork().getAutomaton(cond.first).stateName.at(cond.second);
        if(i < tr.conditions.size()-1) {
            res += ", ";
            i ++;
        }
    }
    res += "} (" + to_string(index) + ")";
    return res;
}





/*----------------------------------------------------------------------------*/
VSolution::VSolution(Graph* pgraph):
Vertex(pgraph)
{
    type = VertexType::SolutionVert;
    graph->addVertex(this);
}

/*----------------------------------------------------------------------------*/
VSolution::VSolution(Graph* pgraph, int autInd, list<int>& trSequence):
Vertex(pgraph)
{
    type = VertexType::SolutionVert;
    graph->addVertex(this);
    updateVTransitions(autInd, trSequence);
}

/*----------------------------------------------------------------------------*/
void VSolution::updateVTransitions(int autInd, list<int>& trSequence) {
    for(auto it = trSequence.begin(); it != trSequence.end(); it ++) {
        VTransition* vtr = graph->getVTransition(autInd, *it);
        suc.push_back(vtr);
        vtr->pred.push_back(this);
    }
}

/*----------------------------------------------------------------------------*/
void VSolution::computeBound(vector<int>& statut, vector<int>& bound) {

    /* solution : sum of transitions */
    bool error = false;
    bound.at(index) = 0;

    for(Vertex* vert : suc) {
        if(bound.at(vert->index) >= 0) {
            bound.at(index) += bound.at(vert->index);
        } else {
            error = true;
        }
    }

    if(error) {
        bound.at(index) = -1;
    }


}

/*----------------------------------------------------------------------------*/
string VSolution::toString() {
    return "o (" + to_string(index) + ")";
}
