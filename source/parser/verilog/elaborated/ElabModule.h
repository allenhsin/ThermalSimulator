#ifndef VERILOG_PARSER_ELAB_MODULE_H
#define VERILOG_PARSER_ELAB_MODULE_H

#include <string>
#include <map>

#include "../VerilogMisc.h"

// A data structure representing an elaborated verilog module
namespace VerilogParser
{
    class ElabItem;
    
    class ElabModule
    {
        public:
            ElabModule(const std::string& name_);
            virtual ~ElabModule();

            // Get the name of the elaborated module
            inline const std::string& getName() const { return m_name_; }		

            // Item map accessors
            bool hasItem(const std::string& identifier_) const;
            void addItem(ElabItem* item_);
            void addItems(ElabItems* items_);            
            const ElabItem* getItem(const std::string& identifier_) const;
            const ElabItemMap& getItemMap() const { return m_item_map_; }
            
            // Clone the elaborated module
            inline virtual ElabModule* clone() const { return new ElabModule(*this); }
            
        // Protected copy constructor, use clone instead
        protected:
            ElabModule(const ElabModule& module_);
            
        private:
            // Module name
            const std::string m_name_;
            // Elaborated items map (mapped by the identifier);
            ElabItemMap m_item_map_;
    };
}
#endif
