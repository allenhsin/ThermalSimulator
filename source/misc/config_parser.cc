
#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <cstring>

#include "source/misc/config_parser.h"
#include "source/misc/common_constants.h"

using std::string;
using std::map;
using std::size_t;
using std::endl;
using std::cerr;

namespace Thermal
{
    ConfigParser::ConfigParser()
    {
        clear();
    }

    ConfigParser::~ConfigParser()
    {}

    void ConfigParser::clear()
    {
        string_data.clear();
        numerical_data.clear();
        boolean_data.clear();
    }

    void ConfigParser::load(const string & config_file_path)
    {
        char    line[LINE_SIZE];
        string  line_copy;
        char*   line_token;

        char    str[STR_SIZE];
        char    current_section_name[STR_SIZE];

        string  config_path;
        char    config_path_temp[STR_SIZE];
        char    config_key_temp[STR_SIZE];

        size_t  found_equal, found_first_quote, found_second_quote;

        FILE* config_file = fopen(config_file_path.c_str(), "r");
        if (!config_file)
        {
            cerr << "\nERROR: Cannot open the config file: " << config_file_path << ".\n" << endl;
            abort();
        }

        while(!feof(config_file))
        {
            // read a line from the file
            fgets(line, LINE_SIZE, config_file);
            if (feof(config_file))
                break;
            
            line_copy = (string) line;
            line_token = strtok(line, " \r\t\n");
            
            // ignore comments and empty lines
            if      (!line_token || line_token[0] == '#')
                continue;
            // new section definition -> [xxx]
            else if (line_token[0] == '[')
            {   
                // make sure the last char is ']'
                do
                {
                    strcpy(str, line_token);
                    line_token = strtok(NULL, " \r\t\n");
                }
                while(line_token);
                if(str[strlen(str)-1]!=']')
                {
                    cerr << "\nERROR: Wrong section definition syntax in config file: " << config_file_path << ".\n" << endl;
                    abort();
                }
                // copy the section name and get rid of '[' and ']'
                strncpy( current_section_name, str+1, (strlen(str)-2));
                current_section_name[strlen(str)-2] = '\0';
            }
            else
            {   
                // locate '=' in the string
                found_equal         = line_copy.find_first_of('=');
                if(found_equal==string::npos)
                {
                    cerr << "\nERROR: Wrong config syntax in config file: " << config_file_path << ".\n" << endl;
                    abort();
                }

                // locate the first double quote in the string
                found_first_quote   = line_copy.find_first_of('\"');
                
                // string type config variable --------------------------------------------------------------
                if (found_first_quote != string::npos)
                {
                    // locate the second double quote in the string
                    found_second_quote  = line_copy.find_first_of('\"', found_first_quote+1);
                    // syntax error if cannot find one
                    if(found_second_quote==string::npos)
                    {
                        cerr << "\nERROR: Wrong config syntax in config file: " << config_file_path << ".\n" << endl;
                        abort();
                    }
                    
                    // config path
                    strcpy(config_path_temp, line_copy.substr(0, found_equal).c_str());
                    line_token = strtok(config_path_temp, " \r\t\n");
                    config_path = (string)current_section_name + "/" + (string)line_token;

                    line_token = strtok(NULL, " \r\t\n");
                    if(line_token)
                    {
                        cerr << "\nERROR: Wrong config syntax in config file: " << config_file_path << ".\n" << endl;
                        abort();
                    }

                    // set in the data structure
                    set(config_path, line_copy.substr(found_first_quote+1, (found_second_quote-found_first_quote-1)));
                }
                // boolean or numerical type config variable ------------------------------------------------
                else
                {
                    if( sscanf(line_copy.c_str(), " %[^= ] =%s", config_path_temp, config_key_temp) == 2)
                    {   
                        config_path = (string)current_section_name + "/" + (string)config_path_temp;
                        // boolean type config variable ----------------------------
                        if      (strcmp(config_key_temp, "true")==0)
                            set(config_path, true);
                        else if (strcmp(config_key_temp, "false")==0)
                            set(config_path, false);
                        // numerical type config variable --------------------------
                        else
                            set(config_path, atof(config_key_temp));
                    }
                    else
                    {
                        cerr << "\nERROR: Wrong config syntax in config file: " << config_file_path << ".\n" << endl;
                        abort();
                    }
                }
                // ------------------------------------------------------------------------------------------
            }
    
        } // while

        fclose(config_file);
    }

    // edit function --------------------------------------------------------------------------------------------------
    void ConfigParser::edit(const std::string & path, const std::string & key)
    {
        ConfigDataType find_result = find(path);

        if      (find_result==TYPE_STRING)
            set(path, key);
        else if (find_result==TYPE_BOOL)
        {   
            if      (key=="true")
                set(path, true);
            else if (key=="false")
                set(path, false);
            else
            {
                cerr << "\nCONFIG ERROR: Wrong config data type: " << path << ".\n" << endl;
                abort();
            }
        }
        else if (find_result==TYPE_NUMBER)
            set(path, atof(key.c_str()));
        else
        {
            cerr << "\nCONFIG ERROR: Wrong config data type: " << path << ".\n" << endl;
            abort();
        }
    }
    // ----------------------------------------------------------------------------------------------------------------

    // set functions --------------------------------------------------------------------------------------------------
    void ConfigParser::set(const std::string & path, const std::string & key)
    {
        ConfigDataType find_result = find(path);

        if( find_result!=TYPE_NON_EXIST && find_result!=TYPE_STRING )
        {
            cerr << "\nCONFIG ERROR: Wrong config data type: " << path << ".\n" << endl;
            abort();
        }
        
        string_data[path] = key;
    }

    void ConfigParser::set(const std::string & path, double key)
    {
        ConfigDataType find_result = find(path);

        if( find_result!=TYPE_NON_EXIST && find_result!=TYPE_NUMBER )
        {
            cerr << "\nCONFIG ERROR: Wrong config data type: " << path << ".\n" << endl;
            abort();
        }
        
        numerical_data[path] = key;
    }

    void ConfigParser::set(const std::string & path, bool key)
    {
        ConfigDataType find_result = find(path);

        if( find_result!=TYPE_NON_EXIST && find_result!=TYPE_BOOL )
        {
            cerr << "\nCONFIG ERROR: Wrong config data type: " << path << ".\n" << endl;
            abort();
        }
        
        boolean_data[path] = key;
    }
    // ----------------------------------------------------------------------------------------------------------------
    
    // get functions --------------------------------------------------------------------------------------------------
    bool ConfigParser::getBool(const std::string & path) const
    {
        ConfigDataType find_result = find(path);

        if(find_result!=TYPE_BOOL)
        {
            cerr << "\nCONFIG ERROR: Cannot find the config data: " << path << ".\n" << endl;
            abort();
        }

        return boolean_data.at(path);
    }

    string ConfigParser::getString(const std::string & path) const
    {
        ConfigDataType find_result = find(path);

        if(find_result!=TYPE_STRING)
        {
            cerr << "\nCONFIG ERROR: Cannot find the config data: " << path << ".\n" << endl;
            abort();
        }

        return string_data.at(path);
    }

    int ConfigParser::getInt(const std::string & path) const
    {
        ConfigDataType find_result = find(path);

        if(find_result!=TYPE_NUMBER)
        {
            cerr << "\nCONFIG ERROR: Cannot find the config data: " << path << ".\n" << endl;
            abort();
        }

        return (int) numerical_data.at(path);
    }

    double ConfigParser::getFloat(const std::string & path) const
    {
        ConfigDataType find_result = find(path);

        if(find_result!=TYPE_NUMBER)
        {
            cerr << "\nCONFIG ERROR: Cannot find the config data: " << path << ".\n" << endl;
            abort();
        }

        return numerical_data.at(path);
    }
    // ----------------------------------------------------------------------------------------------------------------

    ConfigParser::ConfigDataType ConfigParser::find(const std::string & path) const
    {
        ConfigDataType config_data_type = TYPE_NON_EXIST;

        // the path can only exist in one type of data, if it exists
        if      (string_data.count(path)>0)
            config_data_type = TYPE_STRING;
        else if (numerical_data.count(path)>0)
            config_data_type = TYPE_NUMBER;
        else if (boolean_data.count(path)>0)
            config_data_type = TYPE_BOOL;

        return config_data_type;
    }

} // namespace Thermal

