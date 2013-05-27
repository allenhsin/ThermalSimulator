#ifndef VERILOG_PARSER_RAW_MODULE_H
#define VERILOG_PARSER_RAW_MODULE_H

#include <string>
#include <vector>
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class RawModule
    {
        public:
            RawModule(const std::string& name_, const RawItems& items_);
            virtual ~RawModule();

            // Get the item list
            inline const RawItems& getItems() const { return m_items_; }
            // Get the name of the module
            inline const std::string& getName() const { return m_name_; }
		
            // Elaborate the module in the current scope
            void elaborate(VerilogScope* scope_) const;
            // Get string of the module
            std::string toString() const;        
            // Clone a module
            inline RawModule* clone() const { return new RawModule(*this); }

        private:
            // Private copy constructor
            RawModule(const RawModule& module_);
            
            
        private:
            // Module name
            const std::string m_name_;
            // Module items
            RawItems m_items_;
    };
}
#endif
