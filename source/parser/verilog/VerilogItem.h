#ifndef THERMAL_PARSER_VERILOG_ITEM_H
#define THERMAL_PARSER_VERILOG_ITEM_H

#include <string>
#include <vector>

#include "VerilogMisc.h"

// A class representing a verilog module
namespace Thermal
{
    enum VerilogItemType
    {
        ITEM_PARAMETER,
        ITEM_NET,
        ITEM_INSTANCE
    };

    class VerilogItem
    {
        public:
            VerilogItem(VerilogItemType type_);
            virtual ~VerilogItem();
            
        public:
            inline VerilogItemType getType() const { return m_type_; }
            
        private:
            // The type of verilog item this is
            const VerilogItemType m_type_;
    };
}
#endif
