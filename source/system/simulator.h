#ifndef __THERMAL_SIMULATOR_H__
#define __THERMAL_SIMULATOR_H__

#include <stdint.h>
#include <stddef.h>

namespace Thermal
{
    class Simulator
    {
        public:
            static void allocate();
            static void release();
            static Simulator* getSingleton();

            void run(int argc_, char** argv_);
        protected:
            Simulator();
            ~Simulator();


        private:
            static Simulator* _simulator_singleton;
            
            uint64_t _sim_clock; // in ns

    }; // class Simualtor

} // namespace Thermal

# endif // __THERMAL_SIMULATOR_H__
