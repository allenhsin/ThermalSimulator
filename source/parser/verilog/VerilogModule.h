#ifndef THERMAL_PARSER_VERILOG_MODULE_H
#define THERMAL_PARSER_VERILOG_MODULE_H

#include <string>
#include <vector>

// A class representing a verilog module
namespace Thermal
{
    class VerilogNet;
    class VerilogInstance;

    class VerilogModule
    {
        public:
            VerilogModule();
            virtual ~VerilogModule();

            // Get the file name of this file
            
            // Set/get the module name
            void setModuleName(std::string name_) { m_name_ = name_; }
            const std::string getModuleName() { return m_name_; }
            
            // Add a net and get nets
            void addNet(VerilogNet* net_) { m_nets_->push_back(net_); }
            const std::vector<VerilogNet*>* getNets() { return m_nets_; }
            
            // add an instance and get instances
            void addInstance(VerilogInstance* instance_) { m_instances_->push_back(instance_); }
            const std::vector<VerilogInstance*>* getInstances() { return m_instances_; }
            
        private:
            std::vector<VerilogNet*>* m_nets_;
            std::vector<VerilogInstance*>* m_instances_;            
            std::string m_name_;
    };
}
#endif
