#ifndef THERMAL_PARSER_VERILOG_FILE_H
#define THERMAL_PARSER_VERILOG_FILE_H

#include <string>
#include <vector>

// A class representing a verilog file
namespace Thermal
{
    class VerilogModule;

    class VerilogFile
    {
        public:
            VerilogFile(std::string file_name_);
            virtual ~VerilogFile();

            // Get the file name of this file
            inline const std::string& getFileName() { return m_file_name_; }
            
            // add a module to the file
            void addModule(VerilogModule* module_);
            // Get all the modules from a file
            std::vector<VerilogModule*>* getModules();

        private:
            std::vector<VerilogModule*>* m_modules_;
            const std::string m_file_name_;
    };
}
#endif
