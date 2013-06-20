
#ifndef __THERMAL_CONFIG_PARSER_H__
#define __THERMAL_CONFIG_PARSER_H__

#include <string>
#include <map>

namespace Thermal
{   

    class ConfigParser
    {
    public:
        ConfigParser();
        ~ConfigParser();
        
        void clear();
        void load(const std::string & config_file_path);

        void edit(const std::string & path, const std::string & key);

        void set(const std::string & path, const std::string & key);
        void set(const std::string & path, double key);
        void set(const std::string & path, bool key);
        
        bool        getBool(const std::string & path) const;
        std::string getString(const std::string & path) const;
        int         getInt(const std::string & path) const;
        double      getFloat(const std::string & path) const;

    protected:

    private:
        enum ConfigDataType
        {
            TYPE_STRING,
            TYPE_NUMBER,
            TYPE_BOOL,
            TYPE_NON_EXIST
        };

        ConfigDataType find(const std::string & path) const;

        // data structure holding configs
        std::map<std::string, std::string>  string_data;
        std::map<std::string, double>       numerical_data;
        std::map<std::string, bool>         boolean_data;

    }; // class ConfigParser

} // namespadce Thermal

#endif // __THERMAL_CONFIG_PARSER_H__

