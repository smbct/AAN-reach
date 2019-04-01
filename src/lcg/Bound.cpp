/*!
 * \file Bound.cpp
 * \brief implementation of functions to compute a bound on the rechability problem, from the LCG
 * \author S.B
 * \date 18/04/2018
 */

#include <iostream>
#include <stack>
#include <queue>

#include "Graph.hpp"

#include <fstream>

#include <algorithm>

#include <set>

using namespace LCG;
using namespace std;

/*----------------------------------------------------------------------------*/
int Graph::computeBound() {

  /* make sure there is no cycles */

  std::vector<int> statut(_vertices.size(), 0);
  std::vector<int> bounds(_vertices.size(), -1);
  computeBound(_root, statut, bounds);

  return bounds.at(_root->index)+1;
}



/*----------------------------------------------------------------------------*/
void Graph::computeBound(Vertex* vert, vector<int>& statut, vector<int>& bound) {

    // cout << endl << endl << endl << endl;
    // cout << "bound computation for " << vert->toString() << endl;

    /* stack of vertices to process */
    stack<Vertex*> pending;
    pending.push(vert);

    statut.at(vert->index) = 0;

    list<Vertex*> sucList;
    vert->getSuccessors(sucList);

    /* recursively compute the bound */
    while(!pending.empty()) {

        Vertex* top = pending.top();

        // cout << "size: " << pending.size() << endl;
        if(statut.at(top->index) == 2) {
            pending.pop();
        } else if(statut.at(top->index) == 1) { /* compute the bound, all successors already computed */
            /* compute the bound */
            top->computeBound(statut, bound);
            statut.at(top->index) = 2;
            pending.pop();
        } else if(statut.at(top->index) == 0) { /* add the successors in the stack otherwise */

            statut.at(top->index) = 1;
            for(auto suc = top->suc.rbegin(); suc != top->suc.rend(); suc ++) {

                /* check the successors is not already computed */
                if(statut.at((*suc)->index) == 0) {
                    pending.push(*suc);
                }

            }

        }

    }


}
