#ifndef THERMAL_PARSER_VERILOG_PARAMETER_H
#define THERMAL_PARSER_VERILOG_PARAMETER_H

#include <string>
#include <vector>
#include "VerilogItem.h"

// A class representing a verilog module
namespace Thermal
{
    class VerilogParameter : public VerilogItem
    {
        public:
            VerilogParameter(const std::string& name_, const VerilogExpression value_);
            virtual ~VerilogParameter();

            // Get name
            const std::string& getName() const { return m_name_; }
            // Get the value
            const VerilogExpression& getValue() const { return m_value_; }
            
        private:
            const std::string m_name_;
            const VerilogExpression m_value_;
    };
}
#endif
