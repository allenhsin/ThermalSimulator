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
            // Get the verilog scope
            inline const VerilogScope* getVerilogScope() const { return m_scope_; }
            
            // Get a vector of flattened verilog items for a top module, will only return
            // a list of verilog items containing nets and instantiation of primitives
            VerilogItems* getFlattenedItems(const std::string& top_module_name_,
                const std::string& hier_sep_) const;            
            
            // Maps the equivalent nets of a list of verilog items
            static VerilogEquivNetMap* mapEquivNets(const VerilogItems* items_);
            
        private:
            // Helper for get flattened items, adds all items in a module to a list using a hierarchical name
            static void addFlattenedItems(VerilogItems* items_, const std::string& hier_sep_,
                const VerilogModule* module_, const std::string& hier_name_);            
            // Helper for adding equivalenet nets due to instance port connects
            static void addEquivNets(VerilogEquivNetMap* equiv_map_, const VerilogInstance* inst_);
            
        private:
            // List of parsed modules
            VerilogModules* m_modules_;
            // Currently parsed file
            VerilogFile* m_cur_file_;
            // Verilog scope
            VerilogScope* m_scope_;
    };
}
#endif
