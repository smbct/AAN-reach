/*!
 * \file ASP.cpp
 * \brief implementation of ASP.hpp
 * \author S.B
 * \date 24/10/2018
 */

#include "ASP.hpp"

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

 /*---------------------------------------------------------------------------*/
 ASP::ASP(AN& an) :
_an(an)
{

}

/*----------------------------------------------------------------------------*/
bool ASP::reachability(Context& initContext, Context& finalContext, int length) {

  string res = aspNet() + createPath(length);

  /* setup the initial context */
  for(int autInd = 0; autInd < _an.nAutomata(); autInd ++) {
    Automaton& aut = _an.getAutomaton(autInd);
    res += "active(level(\"" + aut.name + "\"," + std::to_string(initContext.at(autInd)) + "),0).\n";
  }

  /* setup the final context */
  for(int autInd = 0; autInd < _an.nAutomata(); autInd ++) {
    if(finalContext.at(autInd) != -1) {
      Automaton& aut = _an.getAutomaton(autInd);
      res += "goal(level(\"" + aut.name + "\"," + std::to_string(finalContext.at(autInd)) + ")).\n";
    }
  }

  res += "#show active/2.\n";

  ofstream file("../expe/TACAS/aspmod.lp");

  if(file) {
    file << res;
    file.close();
  } else {
    cout << "error while writing ASP program" << endl;
  }

  /* run clingo */
  string cmd = "./../solver/clingo-4.5.4-linux-x86_64/clingo";
  cmd += " ../expe/TACAS/aspmod.lp";
  cmd += " > ../expe/TACAS/asp_res";
  // cmd += " --verbose 1";
  int resEx = system(cmd.c_str());

  bool reachable = true;

  ifstream out("../expe/TACAS/asp_res");

  if(out) {

    string line;
    while(!out.eof()) {
      out >> line;
      if(line.find("UNSATISFIABLE") != string::npos) {
        reachable = false;
      }
    }

    out.close();
  }

  return reachable;

}

/*----------------------------------------------------------------------------*/
string ASP::createPath(int length) {

  string res;

  res += "step(0.." + std::to_string(length-2) + ").\n"; /* original encoding modified here*/
  res += "unPlayable(T,S) :- active(level(A,I),S), condition(T,A,J), I!=J, step(S).\n";
  res += "{ played(T,S) } :- not unPlayable(T,S), transition(T), step(S).\n";
  res += ":- 2 { played(_,S) }, step(S).\n";
  // res += ":- 0 { played(_,S) } 0, step(S).\n"; /* modification here, choose if no change is authorized or not */
  res += "change(A,I,J,S) :- played(T,S), target(T,A,J), condition(T,A,I).\n";
  res += ":- X={change(A,I,_,S)}, step(S), automaton(A), X>1.\n";
  res += "active(level(B,K),S+1) :- not change(B,_,_,S), active(level(B,K),S), step(S), S<" + to_string(length) + ".\n";
  res += "active(level(B,K),S+1) :- change(B,_, K, S), S<" + to_string(length) + ".\n";

  return res;
}

/*----------------------------------------------------------------------------*/
string ASP::aspNet() {

  string res;

  int idTr = 0;

  for(int i = 0; i < _an.nAutomata(); i ++) {

    Automaton& aut = _an.getAutomaton(i);

    if(aut.nState > 1) {
      res += "automatonLevel(\"" + aut.name + "\", 0.." + std::to_string(aut.nState-1) + ").\n\n";
    } else {
      res += "automatonLevel(\"" + aut.name + "\"," + std::to_string(aut.nState-1) + ").\n\n";
    }

    /* definition of local states */
    for(Transition& tr : aut.transitions) {

      res += "condition(" + std::to_string(idTr) + ",\"" + aut.name + "\"," + std::to_string(tr.origin) + ").\n";
      res += "target(" + std::to_string(idTr) + ",\"" + aut.name + "\"," + std::to_string(tr.target) + ").\n";

      for(auto cond : tr.conditions) {
        res += "condition(" + std::to_string(idTr) + ",\"" + _an.getAutomaton(cond.first).name + "\"," + std::to_string(cond.second) + ").\n";
      }

      res += "\n";
      idTr ++;
    }

  }

  res += "transition(T) :- target(T,_,_).\n";
  res += "automaton(A) :- automatonLevel(A,_).\n";

  /* goal */
  res += "unReached(Lv,S) :- goal(Lv), not active(Lv, S), step(S).\n";
  res += "reached(S) :- step(S), not unReached(Lv, S), goal(Lv).\n";
  res += "reached :- reached(S).\n";
  res += ":- not reached.\n";

  return res;

}
