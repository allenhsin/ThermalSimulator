
#include "source/system/simulator.h" 
#include "source/system/args_parser.h"
#include "config_file.hpp"

#include <stddef.h>
#include <cassert>

namespace Thermal
{
    Simulator* Simulator::_simulator_singleton = NULL;

    Simulator::Simulator()
        : _sim_clock        (0)
        , _config           (NULL)
        , _data             (NULL)
        , _event_scheduler  (NULL)
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

    void Simulator::run(int argc_, char** argv_)
    {
        // string vector to store parsed command line config arguments
        string_vec  args;
        // default config file if not overridden by command line arguments
        std::string config_file = "./configs/thermal_sim.config";

        // create command line argument parser
        ArgsParser* args_parser = new ArgsParser();
        // create config instance 
        config::ConfigFile* cfg = new config::ConfigFile();


        // Configure Simulator ------------------------------------------------
        // parse command line arguments into args and config_file
        args_parser->parseArgs(args, config_file, argc_, argv_);
        // load config_file config settings into cfg
        cfg->load(config_file);
        // override cfg with command line arguments (if any)
        args_parser->handleArgs(args, *cfg);
        // put cfg into simulator
        _config = cfg;
        // --------------------------------------------------------------------













    }
    

} // namespace Thermal
