#ifndef THERMAL_PARSER_VERILOG_FILE_H
#define THERMAL_PARSER_VERILOG_FILE_H

#include <string>
#include <vector>

#include "VerilogMisc.h"

// A class representing a verilog file
namespace VerilogParser
{
    class VerilogFile
    {
        public:
            VerilogFile(std::string file_name_);
            virtual ~VerilogFile();
            
            // Get the file name of this file
            inline const std::string& getFileName() { return m_file_name_; }

        private:
            // The file name
            const std::string m_file_name_;
    };
}
#endif
