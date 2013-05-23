#ifndef THERMAL_PARSER_VERILOG_NET_H
#define THERMAL_PARSER_VERILOG_NET_H

#include <string>
#include <vector>

#include "VerilogItem.h"
#include "VerilogMisc.h"
#include "VerilogNumber.h"
#include "VerilogExpression.h"

// A class representing a verilog module
namespace VerilogParser
{

    class VerilogScope;

    class VerilogNet : public VerilogItem
    {
        public:
            virtual ~VerilogNet();
            
        private:
            // Private constructor, use the static creation function instead
            VerilogNet(const std::string& identifier_, VerilogPortType port_type_,
                VerilogNetType net_type_, const VerilogRange& range_);

        public:
            static VerilogItems* createVerilogNets(const VerilogVariables& identifiers_, VerilogPortType port_type_,
                VerilogNetType net_type_, const VerilogRange& range_ = VerilogRange(
                        VerilogExpression(VerilogNumber("0")), VerilogExpression(VerilogNumber("0"))));
            
        public:
            inline const VerilogRange& getRange() const { return m_range_; }
            inline VerilogPortType getPortType() const { return m_port_type_; }
            inline VerilogNetType getNetType() const { return m_net_type_; }
            
            std::string toString() const;
            
            // Clone
            VerilogItem* clone() const;
            // Static function for seeing one range is a valid subrange of another
            static bool isValidSubRange(const VerilogRange& range, const VerilogRange& sub_range);
            
        protected:
            void elaborateItem(VerilogScope* scope_);
            // Protected copy constructor, use clone instead
            VerilogNet(const VerilogNet& net_);
            
        private:
            // Port type
            const VerilogPortType m_port_type_;
            // Net type
            const VerilogNetType m_net_type_;
            // The bit range of the net
            VerilogRange m_range_;            
    };
}
#endif
