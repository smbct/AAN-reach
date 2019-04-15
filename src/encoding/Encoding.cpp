/*!
 * \file Encoding.cpp
 * \brief classes and functions to encode Automata network analysis into SAT
 * \author S.B
 * \date 27/03/2018
 */

#include <iostream>
#include <cstdlib>
#include <fstream>

#include "Encoding.hpp"

#include "../interface/Parameters.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
Encoding::Encoding(AN& an):
_an(an),
_verbose(false),
_extractSolution(false)
{

}

/*----------------------------------------------------------------------------*/
void Encoding::setVerbose(bool activate) {
    _verbose = activate;
}

/*----------------------------------------------------------------------------*/
void Encoding::setExtractSolution(bool extract) {
  _extractSolution = extract;
}

/*----------------------------------------------------------------------------*/
void Encoding::createPathVariables(int length) {
    for(int k = 0; k < length; k++) {
        _variables.push_back(StateVar());
        for(int autInd = 0; autInd < _an.nAutomata(); autInd ++) {
            _variables.at(k).push_back(vector<Variable*>());
            for(int stateInd = 0; stateInd < _an.getAutomaton(autInd).nState; stateInd ++) {
                _variables.at(k).at(autInd).push_back(new Variable(_ex));
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
void Encoding::setContext(StateVar& stateVar, Context& context) {
    for(int autInd = 0; autInd < _an.nAutomata(); autInd ++) {
        for(int stateInd = 0; stateInd < static_cast<int>(stateVar.at(autInd).size()); stateInd ++) {
            if(stateInd == context.at(autInd)) { /* the variable must be true */
                _termList.push_back(stateVar.at(autInd).at(stateInd));
            } else if(context.at(autInd) != -1) { /* the variable must be false */
                _termList.push_back(new NotOp(stateVar.at(autInd).at(stateInd)));
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
void Encoding::activateState(StateVar& stateVar) {
    for(int autInd = 0; autInd < _an.nAutomata(); autInd ++) {
        vector<Term*> termList2;
        for(auto var : stateVar.at(autInd)) {
            termList2.push_back(var);
        }
        /* at list one of the local state must be true */
        _termList.push_back(new OrOp(termList2));
    }
}

/*----------------------------------------------------------------------------*/
void Encoding::avoidConflict(StateVar& stateVar) {
    for(int autInd = 0; autInd < _an.nAutomata(); autInd ++) {
        auto aut = _an.getAutomaton(autInd);
        for(int s1 = 0; s1 < aut.nState-1; s1 ++) {
            for(int s2 = s1+1; s2 < aut.nState; s2 ++) {
                _termList.push_back(new NotOp(new AndOp({stateVar.at(autInd).at(s1), stateVar.at(autInd).at(s2)})));
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
void Encoding::createTransition(StateVar& sv1, StateVar& sv2, int k) {
    for(int autInd = 0; autInd < _an.nAutomata(); autInd ++) {
        for(int stateInd = 0; stateInd < _an.getAutomaton(autInd).nState; stateInd ++) {
            activationRule(sv1, sv2, autInd, stateInd, k);
        }
    }

}

/*----------------------------------------------------------------------------*/
void Encoding::activationRule(StateVar& sv1, StateVar& sv2, int autInd, int stateInd, int k) {

    auto aut = _an.getAutomaton(autInd);

    /* list of the transitions */
    vector<Transition*> transitions;

    for(auto& transition : aut.transitions) {
        if(transition.target == stateInd) {
            transitions.push_back(&transition);
        }
    }

    Term* actMode = nullptr;

    if(transitions.empty()) {
        actMode = sv1.at(autInd).at(stateInd);
    } else { /* there are transitions possibilities */

        vector<Term*> transNode;

        /* transition choice, make sure the transition is playable */
        if(transitions.size() == 1) {
            auto trans = transitions.at(0);
            /* origin verified */
            transNode.push_back(sv1.at(autInd).at(trans->origin));
            /* other automatas local state condition */
            for(auto condPair : trans->conditions) {
                transNode.push_back(sv1.at(condPair.first).at(condPair.second));
            }
        } else {
            vector<Term*> condNode;
            /* add all the possible  conditions */
            for(auto trans : transitions) {
                vector<Term*> cond;
                cond.push_back(sv1.at(autInd).at(trans->origin));
                for(auto condPair : trans->conditions) {
                    cond.push_back(sv1.at(condPair.first).at(condPair.second));
                }
                /* choose at least one condition */
                condNode.push_back(new AndOp(cond));
            }
            transNode.push_back(new OrOp(condNode));
        }

        /* make sure the local states of the other automatas are not modified */
        for(int autInd2 = 0; autInd2 < _an.nAutomata(); autInd2 ++) {
            if(autInd2 != autInd) {
                transNode.push_back(noChange(k, autInd2));
            }
        }

        /* make sure the other local states of the same automata are not activated */
        for(int stateInd2 = 0; stateInd2 < aut.nState; stateInd2 ++) {
            if(stateInd2 != stateInd) {
                transNode.push_back(new NotOp(sv2.at(autInd).at(stateInd2)));
            }
        }

        /* create the final node : a transition is done or the same state is kept */
        actMode = new OrOp({sv1.at(autInd).at(stateInd), new AndOp(transNode)});
    }

    /* global node : imply */
    _termList.push_back(new ImplyOp(sv2.at(autInd).at(stateInd), actMode));

}

/*----------------------------------------------------------------------------*/
void Encoding::statesDiff(StateVar& sv1, StateVar& sv2) {
    /* local state activated for sv1 implies not activated for sv2 */
    vector<Term*> orNode; /* choice between automatas: only one must be different */
    for(unsigned int autInd = 0; autInd < sv1.size(); autInd ++) {
        vector<Term*> andNode;
        for(unsigned int stateInd = 0; stateInd < sv1.at(autInd).size(); stateInd ++) {
            andNode.push_back(new ImplyOp(sv1.at(autInd).at(stateInd), new NotOp(sv2.at(autInd).at(stateInd))));
        }
        orNode.push_back(new AndOp(andNode));
    }
    _termList.push_back(new OrOp(orNode));
}

/*----------------------------------------------------------------------------*/
void Encoding::allDiff(vector<StateVar>& variables) {
    for(unsigned int k1 = 0; k1 < variables.size()-1; k1 ++) {
        for(unsigned int k2 = k1+1; k2 < variables.size(); k2 ++) {
            statesDiff(variables.at(k1), variables.at(k2));
        }
    }
}

/*----------------------------------------------------------------------------*/
AndOp* Encoding::noChange(int k, int autInd) {
    if(_noChange.at(k).at(autInd) == nullptr) {
        auto aut = _an.getAutomaton(autInd);
        vector<Term*> nodeList(aut.nState);
        for(int stateInd = 0; stateInd < aut.nState; stateInd ++) {
            nodeList.at(stateInd) = new ImplyOp(_variables.at(k-1).at(autInd).at(stateInd), _variables.at(k).at(autInd).at(stateInd));
        }
        _noChange.at(k).at(autInd) = new AndOp(nodeList);
    }
    return _noChange.at(k).at(autInd);
}

/*----------------------------------------------------------------------------*/
void Encoding::disableState(StateVar& stateVar, int autInd, int stateInd) {
  _termList.push_back(new NotOp(stateVar.at(autInd).at(stateInd)));
}

/*----------------------------------------------------------------------------*/
bool Encoding::reachability(Context& initCtx, Context& finalCtx, int length) {

    Parameters& param = Parameters::getParameters();

    if(param.debugLevel > 0) {
      cout << "create logical variables" << endl;
    }

    createPathVariables(length);

    if(param.debugLevel > 0) {
      cout << "init the noChange vector" << endl;
    }
    _noChange.resize(length, vector<AndOp*>(_an.nAutomata(), nullptr));

    if(param.debugLevel > 0) {
      cout << "set contexts" << endl;
    }

    // Context initCtx, finalCtx;
    // convertContext(strInitContext, initCtx);
    // convertContext(strFinalContext, finalCtx);

    setContext(_variables.at(0), initCtx);
    setContext(_variables.at(length-1), finalCtx);

    if(param.debugLevel > 0) {
      cout << "activate states" << endl;
    }
    for(int k = 0; k < length; k++) {
        activateState(_variables.at(k));
    }

    if(param.debugLevel > 0) {
      cout << "create transitions" << endl;
    }
    for(int k = 0; k < length-1; k ++) {
        createTransition(_variables.at(k), _variables.at(k+1), k+1);
    }

    // cout << "alldiff" << endl;
    // allDiff(_variables);

    // cout << "avoid conflict" << endl;
    // avoidConflict(_variables.at(0));

    if(param.debugLevel > 0) {
      cout << "settle main term" << endl;
    }

    _ex.setMainTerm(new AndOp(_termList));

    vector<cnf::Variable*> cnfVar;

    cnf::CnfExpression cnfEx;

    if(param.debugLevel > 0) {
      cout << "convert to cnf" << endl;
    }
    _ex.toCnf(cnfEx, cnfVar);
    cnfEx.exportDimacs("temp.dm");

    if(param.debugLevel > 0) {
      cout << "nVar : " << cnfEx.nVar() << endl;
      cout << "nClause : " << cnfEx.nClause() << endl;

      cout << "sat solver lunched" << endl;
    }

    /* command for calling SAT solver */
    string cmd;

    // if(param.solver == Parameters::glucose) {
    //   cmd = "./solver/glucose-syrup-4.1/simp/glucose_static";
    // } else if(param.solver == Parameters::minisat) {
    //   cmd = "./solver/minisat/core/minisat_static";
    // } else if(param.solver == Parameters::ccanr) {
    //   cmd = "./solver/CCAnr_2_cai_source_code/CCAnr";
    // } else { /* minisat anyway */
    //   cmd = "./solver/minisat/core/minisat_static";
    // }

    /* solver choice */
    if(param.solver == Parameters::glucose) {
      cmd = "./solver/glucose-syrup-4.1/simp/glucose_static";
    } else { /* minisat anyway */
      cmd = "./solver/minisat/core/minisat";
    }

    // cmd += " -cpu-lim=1";
    // cmd += " --help";

    if(param.solver != Parameters::ccanr) {
      if(param.debugLevel < 2) {
        cmd += " -verb=0";
      }
    }

    if(param.solver == Parameters::ccanr) {
      cmd += " -inst";
    }

    cmd += " temp.dm";

    if(param.solver != Parameters::ccanr) {
      cmd += " res";
    }

    system(cmd.c_str());
    // int res = system("./../solver/minisat/simp/minisat_static test/test.dm test/res");

    /* glucose */
    // int res = system("./../solver/glucose-syrup-4.1/simp/glucose_static test/test.dm test/res");

    bool res = extractSolution(_variables, cnfVar, length);

    if(param.debugLevel > 0) {
      cout << "sat solving done : " << res << endl;
    }

    system("rm res");
    system("rm temp.dm");

    return res;
}

/*----------------------------------------------------------------------------*/
bool Encoding::k_induction(Context& finalCtx, int length) {

  Parameters& param = Parameters::getParameters();

  if(param.debugLevel > 0) {
    cout << "create logical variables" << endl;
  }

  createPathVariables(length);

  if(param.debugLevel > 0) {
    cout << "init the noChange vector" << endl;
  }
  _noChange.resize(length, vector<AndOp*>(_an.nAutomata(), nullptr));

  if(param.debugLevel > 0) {
    cout << "set final goal" << endl;
  }

  setContext(_variables.at(length-1), finalCtx);

  if(param.debugLevel > 0) {
    cout << "disable goal in other states" << endl;
  }
  
  unsigned int autGoal = -1;
  unsigned int stateGoal = -1;

  for(unsigned int i = 0; i < finalCtx.size(); i ++) {
    if(finalCtx.at(i) != -1) {
      autGoal = i;
      stateGoal = finalCtx.at(i);
    }
  }

  /* the goal state should be not active for all other states */
  for(int k = 0; k < length-2; k++) {
    disableState(_variables.at(k), autGoal, stateGoal);
  }

  if(param.debugLevel > 0) {
    cout << "activate states" << endl;
  }
  for(int k = 0; k < length; k++) {
      activateState(_variables.at(k));
  }

  if(param.debugLevel > 0) {
    cout << "create transitions" << endl;
  }
  for(int k = 0; k < length-1; k ++) {
      createTransition(_variables.at(k), _variables.at(k+1), k+1);
  }

  if(param.debugLevel > 0) {
    cout << "alldiff constraint" << endl;
  }
  allDiff(_variables);

  if(param.debugLevel > 0) {
    cout << "avoid conflict csontraint" << endl;
  }
  avoidConflict(_variables.at(0));

  if(param.debugLevel > 0) {
    cout << "settle main term" << endl;
  }

  _ex.setMainTerm(new AndOp(_termList));

  vector<cnf::Variable*> cnfVar;

  cnf::CnfExpression cnfEx;

  if(param.debugLevel > 0) {
    cout << "convert to cnf" << endl;
  }
  _ex.toCnf(cnfEx, cnfVar);
  cnfEx.exportDimacs("temp.dm");

  if(param.debugLevel > 0) {
    cout << "nVar : " << cnfEx.nVar() << endl;
    cout << "nClause : " << cnfEx.nClause() << endl;

    cout << "sat solver lunched" << endl;
  }

  /* command for calling SAT solver */
  string cmd;

  // if(param.solver == Parameters::glucose) {
  //   cmd = "./solver/glucose-syrup-4.1/simp/glucose_static";
  // } else if(param.solver == Parameters::minisat) {
  //   cmd = "./solver/minisat/core/minisat_static";
  // } else if(param.solver == Parameters::ccanr) {
  //   cmd = "./solver/CCAnr_2_cai_source_code/CCAnr";
  // } else { /* minisat anyway */
  //   cmd = "./solver/minisat/core/minisat_static";
  // }

  /* solver choice */
  if(param.solver == Parameters::glucose) {
    cmd = "./solver/glucose-syrup-4.1/simp/glucose_static";
  } else { /* minisat anyway */
    cmd = "./solver/minisat/core/minisat";
  }

  // cmd += " -cpu-lim=1";
  // cmd += " --help";

  if(param.solver != Parameters::ccanr) {
    if(param.debugLevel < 2) {
      cmd += " -verb=0";
    }
  }

  if(param.solver == Parameters::ccanr) {
    cmd += " -inst";
  }

  cmd += " temp.dm";

  if(param.solver != Parameters::ccanr) {
    cmd += " res";
  }

  system(cmd.c_str());
  // int res = system("./../solver/minisat/simp/minisat_static test/test.dm test/res");

  /* glucose */
  // int res = system("./../solver/glucose-syrup-4.1/simp/glucose_static test/test.dm test/res");

  bool res = extractSolution(_variables, cnfVar, length);

  if(param.debugLevel > 0) {
    cout << "sat solving done : " << res << endl;
  }

  system("rm res");
  system("rm temp.dm");

  return res;

}

/*----------------------------------------------------------------------------*/
bool Encoding::extractSolution(vector<StateVar>& stateVar, vector<cnf::Variable*>& cnfVar, int length) {

  Parameters& param = Parameters::getParameters();

    /* values of the cnf variables */
    vector<bool> cnfVal(cnfVar.size());


    fstream file("res");

    if(!file) {
      if(param.debugLevel > 0) {
        cout << "erreur d'ouverture" << endl;
      }
      return false;
    }

    string str;
    file >> str;

    bool res = true;

    if(param.debugLevel > 0) {
      cout << "str: " << str << endl;
    }

    if(_verbose && param.debugLevel > 1) {
      _extractSolution = true;
    }

    if(str == "UNSAT") {
        res = false;
    } else if(str == "SAT" && _extractSolution) {
        while(!file.eof()) {
            file >> str;
            if(str != "0") {
                bool pol = true;
                if(str.at(0) == '-') {
                    pol = false;
                    str = str.substr(1);
                }
                int ind = stoi(str)-1;
                cnfVal.at(ind) = pol;
            }
        }
    }


    // create a table to store nb of occurence of objective use
    std::vector<std::vector<std::vector<int>>> nOcc(_an.nAutomata());
    int maxOcc = 0;

    if(res && _extractSolution) {

        for(int ind = 0; ind < _an.nAutomata(); ind ++) {
          nOcc.at(ind).resize(_an.getAutomaton(ind).nState, std::vector<int>(_an.getAutomaton(ind).nState,0));
        }

        std::vector<int> context(_an.nAutomata(), -1);

        for(int k = 0; k < length; k ++) {

            if(_verbose) {
                cout << "****************************************************" << endl;
                cout << "Global state " << k << endl;
                cout << "****************************************************" << endl;
                cout << endl;
            }

            for(int autInd = 0; autInd < _an.nAutomata(); autInd ++) {

                auto aut = _an.getAutomaton(autInd);

                bool error = false;
                int state = -1;

                for(int stateInd = 0; stateInd < _an.getAutomaton(autInd).nState; stateInd ++) {

                    int varInd = stateVar.at(k).at(autInd).at(stateInd)->index();
                    int cnfInd = cnfVar.at(varInd)->index();

                    if(cnfVal.at(cnfInd)) {
                        if(state == -1) {
                            state = stateInd;
                        } else {
                            error = true;
                        }
                    }

                }

                if(state == -1) {
                    error = true;
                    cout << "whaaat" << endl;
                }

                if(error) {
                    cout << "critical error" << endl;
                    exit(0);
                }

                bool col = false;

                if(k != 0 && context.at(autInd) != state) {
                    col = true;

                    nOcc.at(autInd).at(context.at(autInd)).at(state) ++;
                    if(nOcc.at(autInd).at(context.at(autInd)).at(state) > maxOcc) {
                      maxOcc = nOcc.at(autInd).at(context.at(autInd)).at(state);
                    }
                }

                if(_verbose) {
                    if(col) {
                        cout << "\033[1;32m";
                    }
                    cout << aut.name << " : " << aut.stateName[state];
                    if(col) {
                        cout << "\033[0m";
                    }
                    cout << ", ";
                }
                context.at(autInd) = state;
            }

            if(_verbose) {
                cout << endl << endl;
            }

        }

    }



    return res;
}

/*----------------------------------------------------------------------------*/
void Encoding::convertContext(StrContext& strCtx, Context& ctx) {

    ctx.resize(_an.nAutomata(), -1);

    for(auto& elt : strCtx) {

        int autInd = _an.getAutomatonIndex(elt.first);
        if(autInd != -1) {
            Automaton& aut = _an.getAutomaton(autInd);

            if(aut.stateIndex.find(elt.second) == aut.stateIndex.end()) {
              cout << "fatal error - wrong context for SAT encoding" << endl;
              exit(0);
            } else {
              ctx.at(autInd) = aut.stateIndex[elt.second];
            }

        }
    }

}
