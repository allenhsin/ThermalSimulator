#ifndef VERILOG_PARSER_RAW_ITEM_H
#define VERILOG_PARSER_RAW_ITEM_H

#include <string>
#include <vector>

#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class VerilogScope;

    class RawItem
    {
        public:
            RawItem(RawItemType type_, const std::string& identifier_);
            virtual ~RawItem();
            
        public:        
            inline const std::string& getIdentifier() const { return m_identifier_; }            
            inline RawItemType getType() const { return m_type_; }

            // Elaborate the item in the given verilog scope for the given module
            virtual void elaborate(VerilogScope* scope_) const = 0;
            // Get a string representation of the RawItem
            virtual std::string toString() const;
            // Clone a verilog RawItem
            virtual RawItem* clone() const = 0;            
            
        // Protected copy constructor, use clone instead    
        protected:
            RawItem(const RawItem& item_);
                        
        private:
            // The type of item this is
            const RawItemType m_type_;
            // The identifier for this item
            const std::string m_identifier_;
    };
}
#endif
