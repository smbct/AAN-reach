/*!
* \file ANParser.cpp
* \brief ANParser class implementation
* \author S.B
* \date 13/03/2018
*/

#include "ANParser.hpp"

#include "AN.hpp"

#include <iostream>
#include <regex>

using namespace std;

/*----------------------------------------------------------------------------*/
ANParser::ANParser(std::string content, AN& an) :
_content(content),
_an(an)
{

}

/*----------------------------------------------------------------------------*/
void ANParser::parse() {
  /* remove the comments in the string */
  removeComments();
  /* load the automata */
  loadAutomata();
  /* load the transitions */
  loadTransitions();
  /* load the initial context */
  loadInit();
}

/*----------------------------------------------------------------------------*/
void ANParser::loadInit() {

  regex re("initial_context.*");
  smatch sm;

  /* create a default initial state */
  for(int i = 0; i < _an.nAutomata(); i++)  {
    _an.setInitialState(i, 0);
  }

  /* setup the local states indicated */
  if(regex_search(_content, sm, re)) {

    string line = sm[0];
    // cout << line << endl;
    regex re2("\\s*,?\"?(\\w*)\"?\\s*=\\s*\"?(\\w*)\"?,?\\s*");
    string::const_iterator it(line.cbegin());

    while(regex_search(it, line.cend(), sm, re2)) {
      // cout << sm[0] << " ; *" << sm[1] << "* - *" << sm[2] << "*" << endl;
      int autInd = _an.getAutomatonIndex(sm[1]);
      _an.setInitialState(autInd, _an.getAutomaton(autInd).stateIndex[sm[2]]);
      // _an.setInitialState(sm[1], sm[2]);
      it += sm.position() + sm.length();
    }
  }


}

/*----------------------------------------------------------------------------*/
void ANParser::loadAutomata() {

  // cout << "loading automatas" << endl;

  regex re("(.*\\[.*\\].*)");
  smatch sm;

  string::const_iterator start(_content.cbegin());

  while(regex_search(start, _content.cend(), sm, re)) {
    extractAutomaton(sm[0]);
    start += sm.position() + sm.length();
  }
}


/*----------------------------------------------------------------------------*/
void ANParser::extractAutomaton(std::string line) {

  regex re("([^\\s\"]+)[\"\\s\\[]"); /* catch the name of the automata */

  /* extract automata name */
  string name;
  smatch sm;
  if(regex_search(line, sm, re)) {
    name = sm[1];
  }

  /* create the automata data structure */
  Automaton& automaton = _an.createAutomaton(name);

  /* keep the state part */
  auto p1 = line.find("[");
  auto p2 = line.find("]");
  line = line.substr(p1, p2);

  /* extract automata states */
  regex re2("[\\s,\\[\"]([^\\[\\]\\s,\"])+[\\s,\\]\"]"); /* catch all the states */
  string::const_iterator it(line.cbegin());

  while(regex_search(it, line.cend(), sm, re2)) {

    string state = sm[1];

    /* add the state into the data structure */
    automaton.stateName.push_back(state);
    automaton.stateIndex.insert(std::pair<string, int>(state, automaton.nState));
    automaton.nState += 1;

    it += sm.position() + sm[1].length();
  }
}

/*----------------------------------------------------------------------------*/
void ANParser::loadTransitions() {
  regex re("(.*->.*)");
  smatch sm;

  string::const_iterator start(_content.cbegin());

  while(regex_search(start, _content.cend(), sm, re)) {
    // cout << sm[0] << endl;
    extractTransition(sm[0]);
    start += sm.position() + sm.length();
  }
}

/*----------------------------------------------------------------------------*/
void ANParser::extractTransition(string line) {

  smatch sm;

  /* common transition to copy if synchronised */
  Transition tr;
  tr.origin = -1;
  tr.target = -1;

  /* list of (automata index, transition index) */
  vector<pair<int, int>> shared;

  /* reading the transition conditions */
  regex re("\\s*\"?(\\w*)\"?\\s*=\\s*\"?(\\w*)\"?\\s*");
  string::const_iterator it(line.cbegin());
  while(regex_search(it, line.cend(), sm, re)) {
    int autCondIndex = _an.getAutomatonIndex(sm[1]);
    int stateCondIndex = _an.getAutomaton(autCondIndex).stateIndex[sm[2]];
    tr.conditions.push_back(std::pair<int, int>(autCondIndex, stateCondIndex));
    it += sm.position() + sm.length();
  }

  /* reading the automatas */
  regex re2("\\s*\"?(\\w*)\"?\\s*\"?(\\w*)\"?\\s*->\\s*\"?(\\w*)\"?\\s*");
  string::const_iterator it2(line.cbegin());
  while(regex_search(it2, line.cend(), sm, re2)) {

    /* add a copy of the transition to the corresponding automata */
    int indAut = _an.getAutomatonIndex(sm[1]);
    Automaton& aut = _an.getAutomaton(indAut);
    tr.origin = aut.stateIndex[sm[2]];
    tr.target = aut.stateIndex[sm[3]];
    int indTr = static_cast<int>(aut.transitions.size());
    aut.transitions.push_back(tr);

    shared.push_back(std::pair<int, int>(indAut, indTr));

    it2 += sm.position() + sm.length();
  }

  /* handle synchronous transitions case */
  if(shared.size() > 1) {
    for(unsigned int i = 0; i < shared.size(); i++) {
      for(unsigned int j = 0; j < shared.size(); j++) {
        if(i != j) {
          Automaton& aut = _an.getAutomaton(shared[i].first);
          aut.transitions[shared[i].second].synchro.push_back(shared[j]);
        }
      }
    }
  }


}

/*----------------------------------------------------------------------------*/
void ANParser::removeComments() {
  bool stop = false;
  while(!stop) {
    auto pos1 = _content.find("(**");
    auto pos2 = _content.find("**)");
    if(pos1 != string::npos && pos2 != string::npos) {
      string res = "";
      if(pos1 > 0) {
        res += _content.substr(0, pos1);
      }
      if(pos2+3 < _content.length()) {
        res += _content.substr(pos2+3, _content.length()-pos2+2);
      }
      _content = res;
    } else {
      stop = true;
    }
  }
}
