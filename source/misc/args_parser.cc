#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <stdarg.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include "source/misc/args_parser.h"

namespace Thermal
{
    char* ArgsParser::_prog_name = NULL;

    ArgsParser::ArgsParser()
    {}

    ArgsParser::~ArgsParser()
    {}

    void ArgsParser::usageError(const char* error_msg, ...)
    {
        fprintf(stderr, "Error: ");
        va_list args;
        va_start(args, error_msg);
        vfprintf(stderr,  error_msg, args);
        va_end(args);
        fprintf(stderr, "\n");

        fprintf(stderr, "Usage: %s -c config [extra_options]\n", _prog_name);
        exit(-1);
    }

    void ArgsParser::parseArgs(string_vec &args, std::string & config_path, int argc, char **argv)
    {   
        if (argc != 0)
            _prog_name = argv[0];

        for(int i = 1; i < argc; i++)
        {
            if(strcmp(argv[i], "-c") == 0)
            {
                if(i + 1 >= argc)
                    usageError("Should have provided another argument to the -c parameter.\n");
                config_path = argv[i+1];
                i++;
            }
            else if(strncmp(argv[i],"--config", strlen("--config")) == 0)
            {
                string_vec split_args;
                std::string config_arg(argv[i]);
                boost::split( split_args, config_arg, boost::algorithm::is_any_of("=") );

                if(split_args.size() != 2)
                    usageError("Error parsing argument: %s (%s)\n", config_arg.c_str(), argv[i]);

                config_path = split_args[1];
            }
            else if(strcmp(argv[i], "--") == 0)
                return;
            else if(strncmp(argv[i], "--", strlen("--")) == 0)
                args.push_back(argv[i]);
        }

        if(config_path == "")
            usageError("Should have specified config argument.\n");
        //else
            //fprintf(stderr, "\nRead Config File: %s\n", config_path.c_str());

    }

    void ArgsParser::handleArgs(const string_vec & args, config::ConfigFile & cfg)
    {
        for(string_vec::const_iterator i = args.begin(); i != args.end(); i++)
        {
            string_vec split_args;

            boost::split( split_args, *i, boost::algorithm::is_any_of("=") );

            if( split_args.size() != 2 || split_args[0].size() <= 2 || 
                split_args[0].c_str()[0] != '-' || split_args[0].c_str()[1] != '-')
                usageError("Error parsing argument: %s\n", (*i).c_str());

            std::string setting(split_args[0].substr(2));
            std::string & value(split_args[1]);

            cfg.set(setting, value);
        }
    }

} // namespace Thermal

