#ifndef THERMAL_PARSER_VERILOG_ELAB_ITEM_H
#define THERMAL_PARSER_VERILOG_ELAB_ITEM_H

#include <string>
#include "../VerilogMisc.h"

namespace VerilogParser
{
    // A base class for elaborated verilog items
    class VerilogElabItem
    {
        public:
            VerilogElabItem(VerilogItemType type_, const std::string& identifier_);
            virtual ~VerilogElabItem();
            
        public:
            // Accessor functions
            inline const std::string& getIdentifier() const { return m_identifier_; }            
            inline VerilogItemType getType() const { return m_type_; }

            // Get a clone where all the identifiers have been flattened
            virtual VerilogElabItem* getFlattenedClone(const std::string& hier_, const std::string& hier_sep_) const;            
            // Clone this elaborated verilog item
            virtual VerilogElabItem* clone() const = 0;
            // Get a string representation of the item
            virtual std::string toString() const = 0;

        protected:
            // Protected copy constructor, use clone instead    
            VerilogElabItem(const VerilogElabItem& item_);
                        
        private:
            // The type of verilog item this is
            const VerilogItemType m_type_;
            // The identifier for this item
            std::string m_identifier_;
    };
}
#endif
