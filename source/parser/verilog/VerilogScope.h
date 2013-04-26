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
            
            bool hasModule(std::string name_) const;
            void addModule(VerilogModule* module_);            
            
            VerilogModule* getModule(std::string name_);

            void setModuleScope(VerilogModule* module_);
            inline VerilogModule* getModuleScope() { return m_module_scope_; }
            
        private:
            // Initialize the instance connections
            void initializeConns(const VerilogConns& conns_);

        private:
            // Map of all modules
            VerilogModuleMap m_module_map_;
            // Current module
            VerilogModule* m_module_scope_;
    };
}
#endif
