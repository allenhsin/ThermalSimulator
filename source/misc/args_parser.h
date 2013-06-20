
#ifndef __THERMAL_ARGS_PARSER_H__
#define __THERMAL_ARGS_PARSER_H__

#include "source/misc/config_parser.h"

#include <string>

typedef std::vector< std::string > string_vec;

namespace Thermal
{
    class ArgsParser
    {
    public:
        ArgsParser();
        ~ArgsParser();

        // parse command line input arguments
        void parseArgs(string_vec &args, std::string & config_path, int argc, char **argv);

        // replace configs with command line input arguments
        void handleArgs(const string_vec & args, ConfigParser & cfg);
    
    protected:
        void usageError(const char* error_msg, ...);

    private:
        static char* _prog_name;

    }; // class ArgsParser


} // namespace Thermal



#endif // __THERMAL_ARGS_PARSER_H__
