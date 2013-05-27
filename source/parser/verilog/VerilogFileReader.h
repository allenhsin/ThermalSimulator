#ifndef VERILOG_PARSER_VERILOG_FILE_READER_H
#define VERILOG_PARSER_VERILOG_FILE_READER_H

#include <string>
#include <vector>
#include <ostream>
#include "VerilogMisc.h"

// A class that is able to parse a given verilog file
namespace VerilogParser
{
    class VerilogFile;
    class RawModule;
    class VerilogScope;
    
    class VerilogFileReader 
    {
        public:
            VerilogFileReader();
            virtual ~VerilogFileReader();

            // Parse a file
            bool parse(VerilogFile* file_);
            // Elaborate a top-level module
            void elaborate(const std::string& top_name_);            
            // Get the currently pased file
            inline VerilogFile* getCurFile() { return m_cur_file_; }

            // add a module to the file
            void addModule(RawModule* module_);
            // Get the verilog scope
            inline const VerilogScope* getVerilogScope() const { return m_scope_; }
            
            // Dump raw modules
            void dumpRawModules(std::ostream& ostr_) const;
            // Dump elaborated modules
            void dumpElabModules(std::ostream& ostr_) const;

        private:
            // Currently parsed file
            VerilogFile* m_cur_file_;
            // Verilog scope
            VerilogScope* m_scope_;
    };
}
#endif
