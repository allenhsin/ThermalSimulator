#ifndef THERMAL_PARSER_VERILOG_NET_H
#define THERMAL_PARSER_VERILOG_NET_H

#include <string>
#include <vector>

#include "VerilogItem.h"
#include "VerilogMisc.h"

// A class representing a verilog module
namespace Thermal
{

    enum VerilogPortType
    {
        PORT_INPUT,
        PORT_OUTPUT,
        PORT_INOUT,
        PORT_NONE
    };
    
    // We don't support anything besides regular wires, but we will leave this in for
    // generality
    enum VerilogNetType
    {
        NET_WIRE
    };

    class VerilogNet : public VerilogItem
    {
        public:
            virtual ~VerilogNet();
            
        private:
            // Private constructor, use the static creation function instead
            VerilogNet(const std::string& net_name_, VerilogPortType port_type_,
                VerilogNetType net_type_, const VerilogRange range_);
        public:
            static VerilogItems* createVerilogNets(const VerilogVariables* names_, VerilogPortType port_type_,
                VerilogNetType net_type_, const VerilogRange range_ = VerilogRange(0, 0));
            
        public:
            inline const std::string& getName() const { return m_net_name_; }
            inline const VerilogRange& getRange() const { return m_range_; }
            inline VerilogPortType getPortType() const { return m_port_type_; }
            inline VerilogNetType getNetType() const { return m_net_type_; }
            
        private:
            // Name of the net
            const std::string& m_net_name_;
            // Port type
            const VerilogPortType m_port_type_;
            // Net type
            const VerilogNetType m_net_type_;
            // The bit range of the net
            const VerilogRange& m_range_;            
    };
}
#endif
