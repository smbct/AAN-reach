/*!
 * \file AN.cpp
 * \brief Implementation of AN class
 * \author S.B
 * \date 13/03/2018
 */

#include "AN.hpp"

#include <iostream>

#include <fstream>
#include <sstream>

#include "ANParser.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
string Transition::toString() {

  string res = "";
  res += to_string(origin) + " -> " + to_string(target) + ": ";

  for(auto cond : conditions) {
    res += " " + to_string(cond.first) + "_" + to_string(cond.second);
  }
  return res;
}

/*----------------------------------------------------------------------------*/
AN::AN(string filename) : _filename(filename), _nAut(0) {

    /* loading automata network */
    parse();

}

/*----------------------------------------------------------------------------*/
AN::AN():
_filename("-"),
_nAut(0)
{

}

/*----------------------------------------------------------------------------*/
void AN::parse() {

    ifstream file(_filename);

    if(file) {

        stringstream buffer;
        buffer << file.rdbuf();

        string content = buffer.str();

        /* load the automata network from the file */
        ANParser parser(content, *this);
        parser.parse();

        file.close();
    } else {
        cout << "opening error" << endl;
    }
}

/*----------------------------------------------------------------------------*/
Automaton& AN::createAutomaton(string name) {
    _aut.push_back(Automaton());
    Automaton& aut = _aut.back();

    aut.name = name;
    aut.nState = 0;

    _autIndex.insert(pair<string, int>(name, _nAut));
    _nAut += 1;

    return aut;
}

/*----------------------------------------------------------------------------*/
Automaton& AN::getAutomaton(string name) {
    return _aut[_autIndex[name]];
}

/*----------------------------------------------------------------------------*/
Automaton& AN::getAutomaton(int index) {
    return _aut[index];
}

/*----------------------------------------------------------------------------*/
int AN::getAutomatonIndex(std::string name) {
    if(_autIndex.count(name) > 0) {
        return _autIndex[name];
    } else {
        return -1;
    }
}

/*----------------------------------------------------------------------------*/
int AN::nAutomata() {
    return _nAut;
}

/*----------------------------------------------------------------------------*/
int AN::nLocalStates() {
    int res = 0;
    for(int autInd = 0; autInd < _nAut; autInd ++) {
        res += _aut.at(autInd).nState;
    }
    return res;
}


/*----------------------------------------------------------------------------*/
int AN::nTransitions() {
    int res = 0;
    for(int autInd = 0; autInd < _nAut; autInd ++) {
        res += static_cast<int>(_aut.at(autInd).transitions.size());
    }
    return res;
}


/*----------------------------------------------------------------------------*/
Context& AN::initialContext() {
    return _initContext;
}

/*----------------------------------------------------------------------------*/
void AN::setInitialState(int autInd, int stateInd)
{
    if(_initContext.size() == 0) {
        _initContext.resize(_nAut, -1);
    }
    _initContext.at(autInd) = stateInd;
}

/*----------------------------------------------------------------------------*/
void AN::strContextToContext(StrContext& strCtx, Context& ctx) {

    ctx.resize(_nAut, -1);

    for(auto elt : strCtx) {
        int autInd = _autIndex[elt.first];
        int stateInd = _aut.at(autInd).stateIndex[elt.second];
        ctx.at(autInd) = stateInd;
    }

}

/*----------------------------------------------------------------------------*/
string AN::toString() {

    string res = "";

    for(auto an : _aut) {
        res += "Automaton : '" + an.name + "'\n\t";
        for(int indState = 0; indState < an.nState; indState ++) {
            res += "'" + an.stateName.at(indState) + "'";
            if(indState < an.nState-1) {
                res += ", ";
            }
        }
        res += "\nTransitions :\n";

        for(auto tr : an.transitions) {

            res += "'" + an.stateName.at(tr.origin) + "'";
            res += " -> '" + an.stateName.at(tr.target) + "'";

            for(unsigned int indCond = 0; indCond < tr.conditions.size(); indCond ++) {
                res += " ('";
                int indAutCond = tr.conditions.at(indCond).first;
                res += _aut.at(indAutCond).name;
                res += "' = '";
                res += _aut.at(indAutCond).stateName.at(tr.conditions.at(indCond).second);
                res += "')";
                if(indCond < tr.conditions.size()) {
                    res += " ;";
                }
            }

            res += "\n";

            for(auto p : tr.synchro) {
                res += "'" + _aut.at(p.first).name + "' : ";
                res += to_string(p.second) + " ";
            }
            res += "\n";

        }

        res += "\n\n";
    }


    // res += "initial context : ";
    //
    // for(int autInd = 0; autInd < _nAut; autInd ++) {
    //     if(_initContext.at(autInd) != -1) {
    //         res += _aut.at(autInd).name + "=" + _aut.at(autInd).stateName.at(_initContext.at(autInd)) + " ; ";
    //     }
    // }

    // for(auto it : _initContext) {
    //     res += it.first + "=" + it.second + " ; ";
    // }

    return res;
}
