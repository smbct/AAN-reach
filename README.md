# AAN-reach

AAN-reach is a reachability solver the Asynchronous Automata Network framework. This program encodes the dynamics of Asynchronous Automata Networks into a SAT instance. Then, an external SAT solver is used to look for an execution sequence that verify the reachabillity. In some cases, it is also able to prove the unreachability by computing a sufficient bound on the length of execution sequences. This bound is derived from static analysis tools. For the inconclusive cases, a manual bound can be set to check reachability in bounded sequences.

## How to use it ?

### Operating system

This program has been successfully tested on Ubuntu 18.04.2 LTS.

### Dependencies

The program can use two SAT solvers:

- [minisat](http://minisat.se/MiniSat.html)
- [glucose](http://www.labri.fr/perso/lsimon/glucose/)

These solvers are installed automatically with the installing script. However, you may need the packages **curl** and **zlib1g-dev** (can be installed with the command: sudo apt install "package")

### Installation

First, make the install script executable: sudo chmod +x install.sh. Then, use the command: ./install to download the SAT solvers and compile the program.

### List of parameters

- -s: SAT solver used, minisat or glucose
- -m: Automata Network model path (.an file)
- -i: initial state, for example "a=0,b=0,c=0"
- -g: reachability goal, for example "a=3"
- -b: set a manual bound for the Bounded Model Checking
- -d: debug level, between 0 and 2
- -h: show the help

### Examples

The two following instances are reachable:

- ./aan_reach -s minisat -m "models/example_1.an" -i "a=0,b=0,c=0,d=0" -g "a=3"
- ./aan_reach -s minisat -m "models/example_3.an" -i "a=0,b=0,c=0" -g "a=1"

The following example is not reachable (provable with the bound):

- ./aan_reach -s glucose -m "models/example_3.an" -i "a=0,b=0,c=0" -g "a=2"

The next example is reachable (n states) but the local causality bound cannot be computed:

- ./aan_reach -s minisat -m "models/example_2.an" -i "a=0,b=0,c=0,d=0,e=0" -g "a=1"

A bound can be set manually to compute a reachable sequence:

- ./aan_reach -s minisat -m "models/example_2.an" -i "a=0,b=0,c=0,d=0,e=0" -g "a=1" -b 8

### Documentation

A documentation of the source code can be generated thanks to [Doxygen](http://doxygen.nl/). To generate the documentation, Doxygen must be installed. Then, use the command: doxygen Doxyfile

The documentation is generated in html, in the doc directory. It con be visualized by opening the file index.html in a web browser.  

## Asynchronous Automata Network

Asynchronous Automata Networks is a modeling framework used to represent dynamic systems, such as biological regulatory networks. These models are composed by several automata. Each automaton has several local states and transitions between these local states. The set of (global) states of the model is the cross product of all the local states of the automata. Transitions are also labeled by some local states. A transition can be played from a (global state) if the local states of the transition are present in this state. The Asynchronicity means that from one global state, only one transition can be played. A description of the format can be found [here](https://loicpauleve.name/pint/doc/automata-networks.html).

![An Automata Network](AN.png)

The Automata Network above contains 3 automata with 3 local states for each. (a_1,b_1,c_2) is a global state from which the transition a_1 -> a_2 (b_1, c_2) can be played. The resulting state after playing the transition is (a_2,b_1,c_2).

## Reachability problem

In this framework, the reachability problem is the following question : given an initial global state, is there an execution sequence that leads to a global state with a specific local state ?

For example, in the previous example, the reachability of a_2 from (a_0,b_0,c_0) is possible. Indeed, this local state can be reached after successively playing the transitions : b_0 -> b_1 ; c_0 -> c_2 ; b_1 -> b_2 ; a_0 -> a_1 ; b_2 -> b_1 ; a_1 -> a_2

## Solving reachability through Bounded Model Checking and static analysis

To solve reachability, the program encodes the dynamics of Asynchronous Automata Networks into a propositional logic formula, which is then solved by a SAT solver. The encoding represent an execution sequence of the model. Thus, it is necessary to set the length of the bound.

If the reachability can be done with the chosen length, the SAT solver returns a corresponding reachability sequence. If not, this gives an unreachability proof only for the chosen length. To prove unreachability in the general case, it is necessary to prove that the rechability is not possible even with longer sequences.

The approach used in this program comes from the static analysis tools employed in the solver [Pint](https://loicpauleve.name/pint/). A graph is build to compute a reachability bound. Unfortunately, this bound cannot be computed for every reachability instance.
