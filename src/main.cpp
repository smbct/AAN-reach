#include <iostream>
#include <cstdlib>

#include "interface/Parameters.hpp"
#include "interface/Framework.hpp"

#include "model/AN.hpp"
#include "lcg/Graph.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
int main(int argc, char** argv) {

    // AN network("models/ex12_cycleObl.an");
    //
    // Context initContext(network.nAutomata(), 0);
    //
    // // create initial contex with local state 0
    // for(unsigned int ind = 0; ind < initContext.size(); ind ++) {
    //   initContext.at(ind) = 0;
    // }
    //
    // unsigned int autInd = 0;
    // LocalState goalState(autInd,1);
    //
    // LCG::Graph graph(network, initContext, goalState);
    //
    // graph.build();

    Parameters& param = Parameters::getParameters();

    param.loadFromCmd(argc, argv);

    Framework& framework = Framework::getFramework();

    framework.exec();

    // ./main -e SAT -s glucose -m "../expe/TACAS/egfr104.an" -i "erbb1=0,erbb2=0,erbb3=0,erbb4=0,bir=0,btc=0,egf=0,epr=0" -g "elk1=1"
    //
    // -e SAT -s glucose -m "models/ex7_rep.an" -i "a=0,b=0,c=0,d=0", -g "a=3"

    // ./main -e SAT -s glucose -m "models/ex7_rep.an" -i "a=0,b=0,c=0,d=0" -g "a=3"

    // ./main -e SAT -s glucose -m "models/ex7_rep.an" -i "a=0,b=0,c=0,d=0" -g "a=3"


    return 0;
}
