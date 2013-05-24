#ifndef VERILOG_PARSER_ELAB_NET_H
#define VERILOG_PARSER_ELAB_NET_H

#include <string>
#include "../VerilogMisc.h"
#include "ElabItem.h"

namespace VerilogParser
{
    // An elaborated verilog net
    class ElabNet : public ElabItem
    {
        public:
            // Private constructor, use the static elaborate function instead
            ElabNet(const std::string& identifier_, PortType port_type_,
                NetType net_type_, BitRange range_);
            virtual ~ElabNet();
            
        protected:
            // Protected copy constructor, use clone instead
            ElabNet(const ElabNet& net_);
            
        public:
            // Accessor functions
            inline BitRange getRange() const { return m_range_; }
            inline PortType getPortType() const { return m_port_type_; }
            inline NetType getNetType() const { return m_net_type_; }
            
            // Clone
            ElabItem* clone() const;            
            // Get string representation            
            std::string toString() const;
            
        private:
            // Port type
            const PortType m_port_type_;
            // Net type
            const NetType m_net_type_;
            // Range
            const BitRange m_range_;
    };
}
#endif
