#ifndef THERMAL_PARSER_VERILOG_INSTANCE_H
#define THERMAL_PARSER_VERILOG_INSTANCE_H

#include <string>
#include <vector>
#include "VerilogItem.h"

// A class representing a verilog module
namespace Thermal
{
    class VerilogInstance : public VerilogItem
    {
        public:
            VerilogInstance(const std::string& name_ );
            virtual ~VerilogInstance();
            
            inline const std::string& getName() const { return m_name_; }
            
        private:
            const std::string m_name_;
    };
}
#endif
