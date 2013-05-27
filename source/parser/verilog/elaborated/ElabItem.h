#ifndef VERILOG_PARSER_ELAB_ITEM_H
#define VERILOG_PARSER_ELAB_ITEM_H

#include <string>
#include "../VerilogMisc.h"

namespace VerilogParser
{
    class VerilogScope;

    // A base class for elaborated verilog items
    class ElabItem
    {
        public:
            ElabItem(ElabItemType type_, const std::string& identifier_, const VerilogScope* scope_);
            virtual ~ElabItem();
            
        public:
            // Accessor functions
            inline const std::string& getIdentifier() const { return m_identifier_; }            
            inline const std::string& getPath() const { return m_path_; }
            inline ElabItemType getType() const { return m_type_; }

            // Get a clone where all the identifiers have been flattened
            virtual ElabItem* getFlattenedClone(const std::string& hier_, const std::string& hier_sep_) const;            
            // Clone this elaborated verilog item
            virtual ElabItem* clone() const = 0;
            // Get a string representation of the item
            virtual std::string toString() const;

        protected:
            // Protected copy constructor, use clone instead    
            ElabItem(const ElabItem& item_);
                        
        private:
            // The type of verilog item this is
            const ElabItemType m_type_;
            // The identifier for this item
            std::string m_identifier_;
            // The full path for this item
            const std::string m_path_;
    };
}
#endif
