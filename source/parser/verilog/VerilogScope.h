#ifndef VERILOG_PARSER_VERILOG_SCOPE_H
#define VERILOG_PARSER_VERILOG_SCOPE_H

#include <string>
#include <vector>
#include <map>
#include "VerilogMisc.h"

// Class responsible for taking care of module elaboration
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

            // bool hasElabModule(const std::string& name_) const;
            // void addElabModule(RawModule* module_);                        
            // RawModule* getElabModule(const std::string& name_);
            // inline const RawModuleMap& getElabModules() const { return m_elab_module_map_; }
            
            inline void setModuleScope(ElabModule* module_) { m_module_scope_ = module_; }
            inline ElabModule* getModuleScope() { return m_module_scope_; }

            
            
        private:
            // Global symbol lookup map
            ElabItemsMap m_symbol_map_;            
            // Map of all raw modules
            RawModuleMap m_raw_module_map_;
            // Current module
            ElabModule* m_module_scope_;            
            // Vector of all elaborated moduels
            ElabModules m_elab_modules_;
    };
}
#endif
