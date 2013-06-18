
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <stdarg.h>

#include "source/misc/args_parser.h"
#include "source/misc/common_constants.h"

using std::string;

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

    void ArgsParser::parseArgs(string_vec &args, string & config_path, int argc, char **argv)
    {   
        char arg_name[STR_SIZE];
        char arg_value[STR_SIZE];

        if (argc != 0)
            _prog_name = argv[0];

        for(int i = 1; i < argc; i++)
        {
            if(strcmp(argv[i], "-c") == 0)
            {
                if(i + 1 >= argc)
                    usageError("No config file path argument after -c parameter\n");
                config_path = argv[i+1];
                i++;
            }
            else if(strncmp(argv[i],"--config", strlen("--config")) == 0)
            {   
                if(sscanf(argv[i], "%[^= ]=%s", arg_name, arg_value) != 2)
                    usageError("Error parsing argument: %s\n", argv[i]);

                config_path = arg_value;
            }
            else if(strcmp(argv[i], "--") == 0)
                return;
            else if(strncmp(argv[i], "--", strlen("--")) == 0)
                args.push_back(argv[i]);
        }

        if(config_path == "")
            usageError("Should have specified config argument.\n");
    }

    void ArgsParser::handleArgs(const string_vec & args, config::ConfigFile & cfg)
    {
        char arg_name[STR_SIZE];
        char arg_value[STR_SIZE];

        for(string_vec::const_iterator i = args.begin(); i != args.end(); i++)
        {
            if( sscanf((*i).c_str(), "%[^= ]=%s", arg_name, arg_value) != 2 ||
                strlen(arg_name) <= 2 || arg_name[0] != '-' || arg_name[1] != '-' )
                usageError("Error parsing argument: %s\n", (*i).c_str());

            string setting( arg_name+2  );
            string value(   arg_value   );

            cfg.set(setting, value);
        }
    }

} // namespace Thermal

