#ifndef THERMAL_PARSER_VERILOG_INSTANCE_H
#define THERMAL_PARSER_VERILOG_INSTANCE_H

#include <string>
#include <vector>
#include "VerilogItem.h"
#include "VerilogMisc.h"

// A class representing a verilog module
namespace Thermal
{
    class VerilogScope;
    
    class VerilogInstance : public VerilogItem
    {
        public:
            VerilogInstance(const std::string& identifier_, const VerilogConns& conns_);
            virtual ~VerilogInstance();
            
            inline const VerilogConns* getConns() const { return m_conns_; }            
            inline const VerilogModule* getModule() const { return m_module_; }
            
            // Set the name of the module that this instance instantiates
            inline void setModuleName(const std::string& module_name_ ) { m_module_name_ = module_name_; }
            // Set parameter values
            void setParameterValues(const VerilogConns& conns_);
            
        protected:
            void elaborateItem(VerilogScope* scope_);
            
        private:
            // Initialize the instance connections
            void initializeConns(const VerilogConns& conns_);

        private:
            // Instance connections
            VerilogConns* m_conns_;
            // Instance parameter values
            VerilogConns* m_parameter_values_;
            // Instance module name
            std::string m_module_name_;
            
            // The module that the instance points to
            VerilogModule* m_module_;
            
    };
}
#endif
