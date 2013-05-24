#ifndef VERILOG_PARSER_VERILOG_SCOPE_H
#define VERILOG_PARSER_VERILOG_SCOPE_H

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
            
            // bool hasRawModule(const std::string& name_) const;
            // void addRawModule(RawModule* module_);                        
            // RawModule* getRawModule(const std::string& name_);

            // bool hasElabModule(const std::string& name_) const;
            // void addElabModule(RawModule* module_);                        
            // RawModule* getElabModule(const std::string& name_);
            // inline const RawModuleMap& getElabModules() const { return m_elab_module_map_; }
            
            inline void setModuleScope(RawModule* module_) { m_module_scope_ = module_; }
            inline RawModule* getModuleScope() { return m_module_scope_; }

        private:
            // Map of all raw modules
            RawModuleMap m_raw_module_map_;
            // Map of all elaborated modules
            RawModuleMap m_elab_module_map_;
            // Current module
            RawModule* m_module_scope_;
    };
}
#endif
