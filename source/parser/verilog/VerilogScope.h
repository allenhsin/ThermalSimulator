#ifndef VERILOG_PARSER_VERILOG_SCOPE_H
#define VERILOG_PARSER_VERILOG_SCOPE_H

#include <string>
#include <vector>
#include <map>
#include "VerilogMisc.h"

// Class responsible for taking care of scoping and symbol lookup during
// module elaboration
namespace VerilogParser
{
    class VerilogScope
    {
        public:
            VerilogScope();
            virtual ~VerilogScope();
            
            bool hasRawModule(const std::string& name_) const;
            void addRawModule(RawModule* module_);                        
            RawModule* getRawModule(const std::string& name_);
            const RawModuleMap& getRawModuleMap() const { return m_raw_module_map_; }
            
            inline void setModuleScope(ElabModule* module_) { m_module_scope_ = module_; }
            inline ElabModule* getModuleScope() { return m_module_scope_; }
            
            // Elaborate the verilog for some top-level module name
            void elaborate(const std::string& top_name_);
            
        private:
            // Global symbol lookup map
            IndexedElabItemsMap m_symbol_map_;            

            // Map of all raw modules
            RawModuleMap m_raw_module_map_;
            
            // Current module
            ElabModule* m_module_scope_;            
            // Vector of all elaborated modules
            ElabModules m_elab_modules_;
    };
}
#endif
