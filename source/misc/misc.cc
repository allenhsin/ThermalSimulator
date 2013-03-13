
#include "source/misc/args_parser.h"
#include "config_file.hpp"

namespace Thermal
{
    void Misc::setConfig(std::string config_file, config::Config*& cfg_ref, int argc, char** argv)
    {
        // string vector to store parsed command line config arguments
        string_vec  args;

        // create command line argument parser
        ArgsParser* args_parser = new ArgsParser();
        // create config instance 
        config::ConfigFile* cfg = new config::ConfigFile();

        // parse command line arguments into args and config_file
        args_parser->parseArgs(args, config_file, argc, argv);
        // load config_file config settings into cfg
        cfg->load(config_file);
        // override cfg with command line arguments (if any)
        args_parser->handleArgs(args, *cfg);
        // put cfg into simulator
        cfg_ref = cfg;

        delete args_parser;
    }








} // namespace Thermal


