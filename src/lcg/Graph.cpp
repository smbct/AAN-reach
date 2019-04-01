/*!
 * \file Grpah.cpp
 * \brief implementation of the local causality graph of a reachability problem
 * \author S.B
 * \date 03/04/2018
 */

#include "Graph.hpp"

#include <iostream>
#include <algorithm>

#include <stack>
#include <queue>

#include "../interface/Parameters.hpp"

using namespace std;
using namespace LCG;


/*----------------------------------------------------------------------------*/
Graph::Graph(AN& an, Context& initContext, LocalState& goalState):
_an(an),
_initContext(initContext),
_goalState(goalState),
_root(nullptr)
{
    setupMaps();
}

/*----------------------------------------------------------------------------*/
void Graph::build() {

    Parameters& param = Parameters::getParameters();

    if(param.debugLevel > 0) {
      cout << "creation of the local causality graph" << endl;
    }

    /* add the goal local state */
    _root = new VLocalState(this, _goalState.automaton, _goalState.state);

    cout << "root: " << _root->toString() << endl;

    bool change = false;

    /* build the graph until a fixed point is found */
    do {
        change = false;
        for(unsigned int i = 0; i < _vlocalStates.size(); i ++) {
            if(updateLocalState(_vlocalStates.at(i))) {
              change = true;
          }
        }

        /* check for necessary re-centering */
        change = change || re_centering();

    } while(change);

    cout << "n local states: " << _vlocalStates.size() << endl;
    cout << "n obj: " << _vobjectives.size() << endl;

    bool cycle = checkCycle();
    cout << "cycle ? " << cycle << endl;

    if(!cycle) {
      std::vector<int> statut(_vertices.size(), 0);
      std::vector<int> bounds(_vertices.size(), -1);
      computeBound(_root, statut, bounds);

      cout << "Reachability bound: " << bounds.at(_root->index) << endl;
    }

}


/*----------------------------------------------------------------------------*/
bool Graph::updateLocalState(VLocalState* vlocalState) {

    /* for all local states, corresponding objectives are saturated according to local states from the same automaton */

    bool modif = false;

    /* add objectives to the local state depending on the other local states (frome the same automaton) in the graph */
    for(int stateInd = 0; stateInd < static_cast<int>(_vlocalStateMap.at(vlocalState->autInd).size()); stateInd ++) {

        /* check if the objective has not been created yet */
        if(_vobjectiveMap.at(vlocalState->autInd).at(stateInd).at(vlocalState->stateInd) == nullptr) {
            /* check if the local state exists in the graph, or is the initial state (for all local states, there is an obj. from init local state) */
            if((_vlocalStateMap.at(vlocalState->autInd).at(stateInd) != nullptr) || _initContext.at(vlocalState->autInd) == stateInd) {
                /* the origin of the objective is not the final state */
                if( (vlocalState->autInd != _goalState.automaton) || (stateInd != _goalState.state) ) {
                    VObjective* vobj = new VObjective(this, vlocalState->autInd, stateInd, vlocalState->stateInd);
                    vlocalState->suc.push_back(vobj);
                    vobj->pred.push_back(vlocalState);
                    modif = true;
                }
            }
        }
    }

    return modif;
}


/*----------------------------------------------------------------------------*/
bool Graph::re_centering() {

  bool change = false;

  /* check for necessary re-centering */
  for(unsigned int i = 0; i < _vtransitions.size(); i++) {

      list<Vertex*> suc;
      _vtransitions.at(i)->getSuccessors(suc);
      Automaton& aut = _an.getAutomaton(_vtransitions.at(i)->autInd);
      Transition& tr = aut.transitions.at(_vtransitions.at(i)->trInd);

      for(Vertex* vert : suc) {
          if(vert->type == VertexType::LocalStateVert) {
              VLocalState* vls = static_cast<VLocalState*>(vert);
              if(vls->autInd == _vtransitions.at(i)->autInd) {
                  if(vls->stateInd != tr.origin) {

                      Vertex* rec = getVLocalState(_vtransitions.at(i)->autInd, tr.origin);

                      if(find(_vtransitions.at(i)->suc.begin(), _vtransitions.at(i)->suc.end(), rec) == _vtransitions.at(i)->suc.end()) {

                          /* add an edge from the transition */
                          _vtransitions.at(i)->suc.push_back(rec);
                          rec->pred.push_back(_vtransitions.at(i));
                          change = true;
                      }

                  }
              }
          }
      }
  }

  return change;
}


/*----------------------------------------------------------------------------*/
void Graph::addVertex(Vertex* vertex) {

    vertex->index = static_cast<int>(_vertices.size());
    _vertices.push_back(vertex);

    /* fill the dedicated lists and maps */
    if(vertex->type == LocalStateVert) {
        VLocalState* vlocalState = static_cast<VLocalState*>(vertex);
        _vlocalStates.push_back(vlocalState);
        _vlocalStateMap.at(vlocalState->autInd).at(vlocalState->stateInd) = vlocalState;
    } else if(vertex->type == SolutionVert) {
        _vsolutions.push_back(static_cast<VSolution*>(vertex));
    } else if(vertex->type == ObjectiveVert) {
        VObjective* vobj = static_cast<VObjective*>(vertex);
        _vobjectives.push_back(vobj);
        _vobjectiveMap.at(vobj->autInd).at(vobj->originState).at(vobj->targetState) = vobj;
    } else if(vertex->type == TransitionVert) {
        VTransition* vtransition = static_cast<VTransition*>(vertex);
        _vtransitions.push_back(vtransition);
        _vtransitionMap.at(vtransition->autInd).at(vtransition->trInd) = vtransition;
    }
}

/*----------------------------------------------------------------------------*/
void Graph::setupMaps() {

    _vlocalStateMap.resize(_an.nAutomata());
    for(int autInd = 0; autInd < static_cast<int>(_vlocalStateMap.size()); autInd ++) {
        _vlocalStateMap.at(autInd).resize(_an.getAutomaton(autInd).nState, nullptr);
    }

    _vobjectiveMap.resize(_an.nAutomata());
    for(int autInd = 0; autInd < static_cast<int>(_vobjectiveMap.size()); autInd ++) {
        _vobjectiveMap.at(autInd).resize(_an.getAutomaton(autInd).nState, vector<VObjective*>(_an.getAutomaton(autInd).nState, nullptr));
    }

    _vtransitionMap.resize(_an.nAutomata());
    for(int autInd = 0; autInd < static_cast<int>(_vobjectiveMap.size()); autInd ++) {
        _vtransitionMap.at(autInd).resize(_an.getAutomaton(autInd).transitions.size(), nullptr);
    }
}

/*----------------------------------------------------------------------------*/
AN& Graph::automataNewtork() {
    return _an;
}

/*----------------------------------------------------------------------------*/
int Graph::nVertices() {
    return static_cast<int>(_vertices.size());
}

/*----------------------------------------------------------------------------*/
int Graph::nEdges() {

    int res = 0;
    for(auto vert : _vertices) {
        res += static_cast<int>(vert->suc.size());
    }
    return res;
}

/*----------------------------------------------------------------------------*/
VTransition* Graph::getVTransition(int autInd, int trInd) {

    if(_vtransitionMap.at(autInd).at(trInd) == nullptr) {
        new VTransition(this, autInd, trInd);
    }

    return _vtransitionMap.at(autInd).at(trInd);
}

/*----------------------------------------------------------------------------*/
VLocalState* Graph::getVLocalState(int autInd, int stateInd) {

    if(_vlocalStateMap.at(autInd).at(stateInd) == nullptr) {
        new VLocalState(this, autInd, stateInd);
    }

    return _vlocalStateMap.at(autInd).at(stateInd);
}

/*----------------------------------------------------------------------------*/
Graph::~Graph() {

    for(Vertex* vert : _vertices) {
        delete vert;
    }

}

/*----------------------------------------------------------------------------*/
bool Graph::checkCycle() {

  bool cycle = false;

  /* vertices statut */
  /* 0: unvisited, 1: being visited, succesors added, 2: already visited*/
  vector<unsigned int> statut(_vertices.size(), 0);

  stack<Vertex*> pending;
  pending.push(_root);

  while(!cycle && !pending.empty()) {

    auto top = pending.top();

    if(statut.at(top->index) == 0) {

      statut.at(top->index) = 1;

      for(auto suc: top->suc) {
        if(statut.at(suc->index) == 0) {
          pending.push(suc);
        } else if(statut.at(suc->index) == 1) { /* cycle detected, stop the algorithm */
          cycle = true;
        }
      }

    } else if(statut.at(top->index) == 1) {

      statut.at(top->index) = 2;
      pending.pop();

    } else {
      pending.pop();
    }

  }

  return cycle;
}
