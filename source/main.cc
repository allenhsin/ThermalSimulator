
#include "source/system/simulator.h"

using Thermal::Simulator;

int main(int argc, char** argv)
{
    // create singleton of thermal simulator
    Simulator::allocate();

    // start the thermal simulation
    Simulator::getSingleton()->run(argc-1, argv+1);

    // destroy singleton of thermal simulator
    Simulator::release();

    return 0;
}
