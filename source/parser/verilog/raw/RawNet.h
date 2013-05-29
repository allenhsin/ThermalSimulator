#ifndef VERILOG_PARSER_RAW_NET_H
#define VERILOG_PARSER_RAW_NET_H

#include <string>
#include <vector>

#include "RawItem.h"
#include "../expressions/NumberExpression.h"
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class VerilogScope;

    class RawNet : public RawItem
    {
        public:
            virtual ~RawNet();
            
        private:
            // Private constructor, use the static creation function instead
            RawNet(const std::string& identifier_, PortType port_type_, NetType net_type_, const BitRange& range_);

        public:
            static RawItems* createRawNets(const Identifiers& identifiers_, PortType port_type_,
                NetType net_type_);
            static RawItems* createRawNets(const Identifiers& identifiers_, PortType port_type_,
                NetType net_type_, const BitRange& range_);
            
        public:
            inline const BitRange& getRange() const { return m_range_; }
            inline PortType getPortType() const { return m_port_type_; }
            inline NetType getNetType() const { return m_net_type_; }
            
            // Elaborate the item in the given verilog scope for the given module
            void elaborate(VerilogScope* scope_) const;
            // return as string
            std::string toString() const;            
            // Clone
            RawItem* clone() const;

        protected:
            // Protected copy constructor, use clone instead
            RawNet(const RawNet& net_);
            
        private:
            // Port type
            const PortType m_port_type_;
            // Net type
            const NetType m_net_type_;
            // The bit range of the net
            BitRange m_range_;            
    };
}
#endif
