#ifndef THERMAL_PARSER_VERILOG_MODULE_H
#define THERMAL_PARSER_VERILOG_MODULE_H

#include <string>
#include <vector>
#include "VerilogMisc.h"

// A class representing a verilog module
namespace Thermal
{
    class VerilogNet;
    class VerilogInstance;
    
    class VerilogModule
    {
        public:
            VerilogModule(const std::string& name_, const VerilogItems* items_);
            virtual ~VerilogModule();

            // Get the module name
            const std::string getName() const { return m_name_; }            
            // Get a net
            const VerilogNets* getNets() const { return m_nets_; }            
            // Get an instance
            const VerilogInstances* getInstances() const { return m_instances_; }
            
        private:
            // Initialize the the module based on the module items
            void initializeItems(const VerilogItems* items_);
            
        private:
            VerilogNets* m_nets_;
            VerilogInstances* m_instances_;            
            const std::string m_name_;
    };
}
#endif
