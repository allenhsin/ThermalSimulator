#ifndef THERMAL_PARSER_VERILOG_PARAMETER_H
#define THERMAL_PARSER_VERILOG_PARAMETER_H

#include <string>
#include <vector>
#include "VerilogItem.h"
#include "VerilogExpression.h"

// A class representing a verilog module
namespace Thermal
{
    class VerilogScope;

    class VerilogParameter : public VerilogItem
    {
        public:
            VerilogParameter(const std::string& identifier_, const VerilogExpression& value_);
            virtual ~VerilogParameter();

        public:
            // Get the value
            const VerilogExpression& getValue() const { return m_value_; }
            
        protected:
            void elaborateItem(VerilogScope* scope_);
            
        private:
            VerilogExpression m_value_;
    };
}
#endif
