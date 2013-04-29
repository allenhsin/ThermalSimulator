#ifndef THERMAL_PARSER_VERILOG_FILE_READER_H
#define THERMAL_PARSER_VERILOG_FILE_READER_H

#include <string>
#include <vector>
#include "VerilogMisc.h"

// A class that is able to parse a given verilog file
namespace VerilogParser
{
    class VerilogFile;
    class VerilogModule;
    
    class VerilogFileReader 
    {
        public:
            VerilogFileReader();
            virtual ~VerilogFileReader();

            // Parse a file
            bool parse(VerilogFile* file_);
            // Elaborate parsed files
            void elaborate();            
            // Get the currently pased file
            inline VerilogFile* getCurFile() { return m_cur_file_; }

            // add a module to the file
            void addModule(VerilogModule* module_);
            // Get all the modules from a file
            inline const VerilogModules* getModules() const { return m_modules_; }

        private:
            // List of parsed modules
            VerilogModules* m_modules_;
            // Currently parsed file
            VerilogFile* m_cur_file_;
    };
}
#endif
