#ifndef VERILOG_PARSER_ELAB_ITEM_H
#define VERILOG_PARSER_ELAB_ITEM_H

#include <string>
#include "../VerilogMisc.h"

namespace VerilogParser
{
    // A base class for elaborated verilog items
    class ElabItem
    {
        public:
            ElabItem(ElabItemType type_, const std::string& identifier_);
            virtual ~ElabItem();
            
        public:
            // Accessor functions
            inline const std::string& getIdentifier() const { return m_identifier_; }            
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
    };
    
    // An ElabItems vector with indexed items that have an offset in them
    class IndexedElabItems
    {
        public:
            IndexedElabItems(int idx_offset_);
            ~IndexedElabItems();
            
        public:
            // Add an item to the back
            inline void addBack(ElabItem* item_) { m_items_.push_back(item_); }
            // Bit index accessors
            inline ElabItem*& operator[](int idx_) { return m_items_[idx_ + m_idx_offset_]; }
            inline const ElabItem* operator[](int idx_) const { return m_items_[idx_ + m_idx_offset_]; }
            
        private:
            const int m_idx_offset_;
            ElabItems m_items_;
    };
}
#endif
