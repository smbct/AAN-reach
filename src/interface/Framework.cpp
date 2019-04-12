/*!
 * \file Framework.cpp
 * \brief implementation of the Framework class
 * \author S.B
 * \date 26/10/2018
 */

#include <iostream>

#include "Framework.hpp"
#include "Parameters.hpp"
#include "../model/AN.hpp"
#include "../encoding/Encoding.hpp"
#include "../asp/ASP.hpp"
#include "../lcg/Graph.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
Framework& Framework::getFramework() {
  static Framework framework;
  return framework;
}

/*----------------------------------------------------------------------------*/
Framework::Framework() {

}

/*----------------------------------------------------------------------------*/
void Framework::exec() {

  Parameters& param = Parameters::getParameters();

  if(param.help) { /* show the help */
    param.showHelp();
  } else { /* lunch the reachability solver */
    reachability();
  }

}

/*----------------------------------------------------------------------------*/
void Framework::reachability() {

  Parameters& param = Parameters::getParameters();

  if(param.debugLevel > 1) {
    param.display();
  }

  if(param.encoding != Parameters::undefined) {

    AN model(param.model);

    /* extract goal */
    if(param.goal.size() != 1) {
      cout << "error, no time to implement this feature" << endl;
      exit(0);
    }

    /* identification of the goal state */
    LocalState goal;
    {
      auto it = param.goal.begin();
      int autInd = model.getAutomatonIndex(it->first);
      auto automaton = model.getAutomaton(autInd);
      goal.automaton = autInd;
      goal.state = automaton.stateIndex[it->second];
    }

    /* creation of the final context */
    Context finalCtx(model.nAutomata(),-1);
    finalCtx.at(goal.automaton) = goal.state;

    /* Creation of the initial context */
    Context initCtx = model.initialContext();
    for(auto elt : param.initialState) {
      int autInd = model.getAutomatonIndex(elt.first);
      Automaton& aut = model.getAutomaton(autInd);
      initCtx.at(autInd) = aut.stateIndex[elt.second];
    }

    /* create the LCG */
    LCG::Graph lcg(model, initCtx, goal);
    lcg.build();

    bool reachable = false;

    if(lcg.checkCycle()) {

      cout << "The Local Causality Graph contains cycles, the bound cannot be computed." << endl;

    } else {

      unsigned int bound = lcg.computeBound();

      cout << "Local causality bound: " << bound << " state(s)" << endl;

      // if(param.encoding == Parameters::SAT) {
      Encoding en(model);
      if(param.debugLevel >= 1) {
        en.setVerbose(true);
      }
      reachable = en.reachability(initCtx, finalCtx, bound);
      // } else if(param.encoding == Parameters::ASP) {
      //   ASP aspEnc(model);
      //   reachable = aspEnc.reachability(initCtx, finalCtx, bound+1);
      // }

      cout << endl << "Result: ";
      if(reachable) {
        cout << "reachable" << endl;
      } else {
        cout << "unreachable" << endl;
      }
    }


  }

}
