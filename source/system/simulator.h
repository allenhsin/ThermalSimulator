#ifndef __THERMAL_SIMULATOR_H__
#define __THERMAL_SIMULATOR_H__

#include <stdint.h>
#include <vector>

#include "source/data/data.h"
#include "source/models/model.h"
#include "source/system/event_scheduler.h"
#include "config.hpp"

namespace Thermal
{
    class Simulator
    {
    public:
        // create simulator singleton
        static void allocate();
        // destroy simulator singleton
        static void release();
        // get simulator singleton pointer
        static Simulator* getSingleton();
        // start to run simulation
        void run(int argc_, char** argv_);
        // get config pointer
        config::Config* getConfig() { return _config; }

    
    protected:
        Simulator();
        ~Simulator();

    private:
        static Simulator*       _simulator_singleton;
        
        config::Config*         _config;

        Data*                   _data;
        EventScheduler*         _event_scheduler;
        std::vector< Model* >   _model_vec;
        
        double                  _sim_clock; // in ps

    }; // class Simualtor

} // namespace Thermal

# endif // __THERMAL_SIMULATOR_H__
