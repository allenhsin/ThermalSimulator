#ifndef THERMAL_PARSER_VERILOG_SCOPE_H
#define THERMAL_PARSER_VERILOG_SCOPE_H

#include <string>
#include <vector>
#include <map>
#include "VerilogMisc.h"

// Simple class containing the current elaboration scope
namespace VerilogParser
{
    class VerilogScope
    {
        public:
            VerilogScope();
            virtual ~VerilogScope();
            
            bool hasRawModule(const std::string& name_) const;
            void addRawModule(VerilogModule* module_);                        
            VerilogModule* getRawModule(const std::string& name_);

            bool hasElabModule(const std::string& name_) const;
            void addElabModule(VerilogModule* module_);                        
            VerilogModule* getElabModule(const std::string& name_);
            inline const VerilogModuleMap& getElabModules() const { return m_elab_module_map_; }
            
            inline void setModuleScope(VerilogModule* module_) { m_module_scope_ = module_; }
            inline VerilogModule* getModuleScope() { return m_module_scope_; }
            
        private:
            // Initialize the instance connections
            void initializeConns(const VerilogConns& conns_);

        private:
            // Map of all raw modules
            VerilogModuleMap m_raw_module_map_;
            // Map of all elaborated modules
            VerilogModuleMap m_elab_module_map_;
            // Current module
            VerilogModule* m_module_scope_;
    };
}
#endif
