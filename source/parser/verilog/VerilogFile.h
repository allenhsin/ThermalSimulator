#ifndef THERMAL_PARSER_VERILOG_FILE_H
#define THERMAL_PARSER_VERILOG_FILE_H

#include <string>
#include <vector>

#include "VerilogMisc.h"

// A class representing a verilog file
namespace VerilogParser
{
    class VerilogModule;
    class VerilogScope;
    
    class VerilogFile
    {
        public:
            VerilogFile(std::string file_name_);
            virtual ~VerilogFile();

            // Elaborate the file
            void elaborate(VerilogScope* scope_);
            
            // Get the file name of this file
            inline const std::string& getFileName() { return m_file_name_; }
            
            // add a module to the file
            void addModule(VerilogModule* module_);
            // Get all the modules from a file
            inline const VerilogModules* getModules() const { return m_modules_; }

        private:
            VerilogModules* m_modules_;
            const std::string m_file_name_;
    };
}
#endif
