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
            ElabModule(const std::string& name_, const SymbolMap& params_);
            virtual ~ElabModule();

            // Get the name of the elaborated module
            inline const std::string& getName() const { return m_name_; }		
            // Get the parameter settings of the module
            inline const SymbolMap& getParams() const { return m_params_; }
            // Item map accessors
            // bool hasItem(const std::string& identifier_) const;
            inline void addItem(const ElabItem* item_) { m_items_.push_back(item_); }
            // void addItems(ElabItems* items_);            
            // const ElabItem* getItem(const std::string& identifier_) const;
            const ElabItems& getItems() const { return m_items_; }
            
            // Clone the elaborated module
            inline virtual ElabModule* clone() const { return new ElabModule(*this); }
            // Get string of the module
            std::string toString() const;     
            
        // Protected copy constructor, use clone instead
        protected:
            ElabModule(const ElabModule& module_);
            
        private:
            // Module name
            const std::string m_name_;
            // Parameters
            const SymbolMap m_params_;
            // Elaborated items map (mapped by the identifier);
            ElabItems m_items_;
    };
}
#endif
