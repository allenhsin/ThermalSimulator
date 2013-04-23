#ifndef THERMAL_PARSER_VERILOG_FILE_READER_H
#define THERMAL_PARSER_VERILOG_FILE_READER_H

#include <string>
#include <vector>

// A class that is able to parse a given verilog file
namespace Thermal
{
    class VerilogFile;
    
    class VerilogFileReader 
    {
        public:
            VerilogFileReader(VerilogFile* file_);
            virtual ~VerilogFileReader();

            // Read the file
            bool parse();   
            // Get a pointer to the file data structure
            inline const VerilogFile* getFile() { return m_file_; }

        private:
            VerilogFile* m_file_;
    };
}
#endif