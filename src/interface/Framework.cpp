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
    if(param.k_induction) {
      k_induction();
    } else {
      reachability();
    }
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
      cout << "error, this feature is not implemented" << endl;
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

    /* Local Causality Graph (for the bound) */
    LCG::Graph lcg(model, initCtx, goal);

    if(param.bound <= 0) { /* create the LCG */
      lcg.build();
    }

    bool reachable = false;

    bool lcg_cycles;
    if(param.bound <= 0) {
      lcg_cycles = lcg.checkCycle();
    }

    if(param.bound <= 0 && lcg_cycles) {

      cout << "The Local Causality Graph contains cycles, the bound cannot be computed." << endl;

    } else {

      unsigned int bound = -1;

      if(param.bound <= 0 && !lcg_cycles) {
        bound = lcg.computeBound();
        cout << "Local causality bound: " << bound << " state(s)" << endl;

      } else {
        cout << "Bound manually set to " << param.bound << endl;
      }


      // if(param.encoding == Parameters::SAT) {
      Encoding en(model);
      if(param.debugLevel >= 1) {
        en.setVerbose(true);
      }
      if(param.bound <= 0) {
        reachable = en.reachability(initCtx, finalCtx, bound);
      } else {
        reachable = en.reachability(initCtx, finalCtx, param.bound);
      }
      // } else if(param.encoding == Parameters::ASP) {
      //   ASP aspEnc(model);
      //   reachable = aspEnc.reachability(initCtx, finalCtx, bound+1);
      // }

      cout << endl << "Result: ";
      if(reachable) {
        cout << "reachable" << endl;
      } else {
        if(param.bound <= 0) {
          cout << "unreachable" << endl;
        } else {
          cout << "unreachable for sequences of length " << param.bound << endl;
          cout << "Inconclusive in the general case" << endl;
        }
      }
    }


  }

}

/*----------------------------------------------------------------------------*/
void Framework::k_induction() {

  Parameters& param = Parameters::getParameters();

  if(param.debugLevel > 1) {
    param.display();
  }


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


    bool satisfiable = false;

    // if(param.encoding == Parameters::SAT) {
    Encoding en(model);
    if(param.debugLevel >= 1) {
      en.setVerbose(true);
    }

    cout << "bound: " << param.bound << endl;

    if(param.bound >= 2) {
      satisfiable = en.k_induction(finalCtx, param.bound);
      cout << endl << "Result: the length ";
      if(satisfiable) {
        cout << param.bound << " may not be a completeness bound for that instance" << endl;
      } else {
        cout << param.bound << " is a completeness bound for that instance" << endl;
      }
    } else {
      cout << "Error, the bound has not been set correctly" << endl;
    }



}
