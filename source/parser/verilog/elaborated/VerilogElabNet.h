#ifndef THERMAL_PARSER_VERILOG_ELAB_NET_H
#define THERMAL_PARSER_VERILOG_ELAB_NET_H

#include <string>
#include "../VerilogMisc.h"
#include "VerilogElabItem.h"

namespace VerilogParser
{
    // An elaborated verilog net
    class VerilogElabNet : public VerilogElabItem
    {
        public:
            // Private constructor, use the static elaborate function instead
            VerilogElabNet(const std::string& identifier_, VerilogPortType port_type_,
                VerilogNetType net_type_, const VerilogElabRange& range_);
            virtual ~VerilogElabNet();
            
        public:
            // Accessor functions
            inline VerilogElabRange getRange() const { return m_range_; }
            inline VerilogPortType getPortType() const { return m_port_type_; }
            inline VerilogNetType getNetType() const { return m_net_type_; }
            
            // Clone
            VerilogElabItem* clone() const;            
            // Get string representation            
            std::string toString() const;
            
        protected:
            // Protected copy constructor, use clone instead
            VerilogElabNet(const VerilogElabNet& net_);
            
        private:
            // Port type
            const VerilogPortType m_port_type_;
            // Net type
            const VerilogNetType m_net_type_;
            // Range
            const VerilogElabRange m_range_;
    };
}
#endif
