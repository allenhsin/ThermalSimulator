#include "source/system/simulator.h" 

#include <stddef.h>
#include <cassert>

namespace Thermal
{
    Simulator* Simulator::_simulator_singleton = NULL;

    Simulator::Simulator()
        : _sim_clock(0)
    {}

    Simulator::~Simulator()
    {}

    void Simulator::allocate()
    {
        assert(_simulator_singleton==NULL);
        _simulator_singleton = new Simulator();
        assert(_simulator_singleton);
    }

    void Simulator::release()
    {
        assert(_simulator_singleton);
        delete _simulator_singleton;
        _simulator_singleton = NULL;
    }
    
    Simulator* Simulator::getSingleton()
    { return _simulator_singleton; }

    void Simulator::run(int argc_, char** argv)
    {}
    

} // namespace Thermal
