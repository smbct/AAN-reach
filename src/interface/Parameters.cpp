/*!
 * \file interface.cpp
 * \brief functions for command line usage
 * \author S.B
 * \date 25/10/2018
 */

#include <iostream>
#include <regex>

#include "Parameters.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
Parameters::Parameters() {
  solver = minisat;
  encoding = SAT;
  debugLevel = 0;
  help = false;
}

/*----------------------------------------------------------------------------*/
Parameters& Parameters::getParameters() {
  static Parameters inst;
  return inst;
}

/*----------------------------------------------------------------------------*/
void Parameters::loadFromCmd(int argc, char** argv) {

  int i = 1;
  while(i < argc) {

    string token = argv[i];

    if(token == "-m") {
      model = string(argv[i+1]);
    } else if(token == "-e") {
      if(string(argv[i+1]) == "SAT") {
        encoding = Parameters::SAT;
      } else if(string(argv[i+1]) == "ASP") {
        encoding = Parameters::ASP;
      }
    } else if(token == "-i") {
      extractContext(argv[i+1], initialState);
    } else if(token == "-g") {
      extractContext(argv[i+1], goal);
    } else if(token == "-h") {
      showHelp();
    } else if(token == "-d") {
      debugLevel = stoi(argv[i+1]);
    } else if(token == "-s") {
      if(string(argv[i+1]) == "minisat") {
        solver = minisat;
      } else if(string(argv[i+1]) == "glucose") {
        solver = glucose;
      } else if(string(argv[i+1]) == "maplecomsps") {
        solver = maplecomsps;
      } else if(string(argv[i+1]) == "ccanr") {
        solver = ccanr;
      }
    } else if(token == "-h") {
      help = true;
    } else {
      i --;
    }

    i += 2;

  }

}

/*----------------------------------------------------------------------------*/
void Parameters::extractContext(string token, StrContext& ctx) {

  regex re("\\s*,?\"?(\\w*)\"?\\s*=\\s*\"?(\\w*)\"?,?\\s*");
  smatch sm;
  string::const_iterator it(token.cbegin());

  while(regex_search(it, token.cend(), sm, re)) {

      // int autInd = _an.getAutomataIndex(sm[1]);

      // cout << sm[1] << " -> " << sm[2] << endl;

      ctx[sm[1]] = sm[2];

      // _an.setInitialState(autInd, _an.getAutomata(autInd).stateIndex[sm[2]]);
      // _an.setInitialState(sm[1], sm[2]);
      it += sm.position() + sm.length();
  }

}

/*----------------------------------------------------------------------------*/
void Parameters::display() {

  cout << endl << endl;

  cout << "PARAMETERS:" << endl << endl;

  cout << "Encoding:";
  cout << "\t- ";
  if(encoding == Parameters::SAT) {
    cout << "SAT" << endl;
  } else if(encoding == Parameters::ASP) {
    cout << "ASP" << endl;
  }

  if(encoding == Parameters::SAT) {
    cout << "Solver:";
    cout << "\t- ";
    if(solver == minisat) {
      cout << "minisat" << endl;
    } else if(solver == glucose) {
      cout << "glucose" << endl;
    } else if(solver == maplecomsps) {
      cout << "maplecomsps" << endl;
    } else if(solver == ccanr) {
      cout << "ccanr" << endl;
    } else {
      cout << "error" << endl;
    }
  }

  cout << "Model:";
  cout << "\t- " << model << endl;

  cout << "Initial state:";
  cout << "\t- ";
  for(auto elt : initialState) {
    cout << elt.first << "=" << elt.second << ", ";
  }
  cout << endl;

  cout << "Goal:";
  cout << "\t- ";
  for(auto elt : goal) {
    cout << elt.first << "=" << elt.second << ", ";
  }
  cout << endl;

  cout << "Debug level:\t- " << debugLevel << endl;

  cout << endl << endl << endl;

}

/*----------------------------------------------------------------------------*/
void Parameters::showHelp() {

  cout << "*****************************************************************" << endl;
  cout << "*************************** AAN-reach ***************************"  << endl;
  cout << "*********Reachability in Asynchronous Automata Network***********" << endl;
  cout << "*****************************************************************" << endl;

  cout << endl << "Command line usage: " << endl << endl;

  cout << "./aan_reach -m model.an -i initial_state -g goal_state -s SAT_solver " << endl << endl;

  cout << "Parameters: " << endl << endl;

  cout << "-m : input automata network model" << endl;
  cout << "-h : show this help" << endl;
  // cout << "-e : encoding : SAT or ASP" << endl;
  cout << "-i : initial local state, ex: \"a=0, b=1, c=0\"" << endl;
  cout << "-g : goal state, ex: \"a=2\"" << endl;
  cout << "-s : sat solver: minisat, glucose" << endl << endl;
}
