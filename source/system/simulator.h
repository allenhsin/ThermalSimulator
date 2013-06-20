#ifndef __THERMAL_SIMULATOR_H__
#define __THERMAL_SIMULATOR_H__

#include <stdint.h>
#include <vector>

#include "source/data/data.h"
#include "source/models/model.h"
#include "source/models/model_type.h"
#include "source/system/event_scheduler.h"
#include "source/misc/config_parser.h"

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
        
        // get module accesses
        ConfigParser* getConfig()         { return _config; }
    
    protected:
        Simulator();
        ~Simulator();

        void finalize();

    private:
        static Simulator*       _simulator_singleton;
        
        ConfigParser*         _config;

        Data*                   _data;
        EventScheduler*         _event_scheduler;
        std::vector< Model* >   _model;

    }; // class Simualtor

} // namespace Thermal

# endif // __THERMAL_SIMULATOR_H__
