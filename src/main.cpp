#include <iostream>
#include <cstdlib>

#include "interface/Parameters.hpp"
#include "interface/Framework.hpp"

#include "model/AN.hpp"
#include "lcg/Graph.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
int main(int argc, char** argv) {

    Parameters& param = Parameters::getParameters();

    param.loadFromCmd(argc, argv);

    Framework& framework = Framework::getFramework();

    framework.exec();

    return 0;
}
