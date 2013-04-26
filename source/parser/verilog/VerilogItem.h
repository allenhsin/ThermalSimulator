#ifndef THERMAL_PARSER_VERILOG_ITEM_H
#define THERMAL_PARSER_VERILOG_ITEM_H

#include <string>
#include <vector>

#include "VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class VerilogScope;

    enum VerilogItemType
    {
        ITEM_PARAMETER,
        ITEM_NET,
        ITEM_INSTANCE
    };

    class VerilogItem
    {
        public:
            VerilogItem(VerilogItemType type_, const std::string& identifier_);
            virtual ~VerilogItem();
            
            void elaborate(VerilogScope* scope_);

        protected:
            // Override this function instead of the regular elaborate function
            virtual void elaborateItem(VerilogScope* scope_) = 0;
        
        public:
            inline const std::string& getIdentifier() const { return m_identifier_; }
            
        public:
            inline VerilogItemType getType() const { return m_type_; }
            virtual std::string toString() const = 0;
            
        private:
            // The type of verilog item this is
            const VerilogItemType m_type_;
            // The identifier for this item
            const std::string m_identifier_;
            // Whether this item has been elaborated
            bool m_elaborated_;
    };
}
#endif
