
#ifndef __THERMAL_MISC_H__
#define __THERMAL_MISC_H__

#include <string>

#include "config.hpp"

namespace Thermal
{
    class Misc
    {
    public:
        static void setConfig(std::string config_file, config::Config*& cfg_ref, int argc, char** argv);

    }; // class Misc

} // namespace Thermal

#endif // __THERMAL_MISC_H__

