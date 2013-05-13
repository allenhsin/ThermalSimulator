#ifndef THERMAL_PARSER_VERILOG_MODULE_H
#define THERMAL_PARSER_VERILOG_MODULE_H

#include <string>
#include <map>

#include "../VerilogMisc.h"

// A data structure representing an elaborated verilog module
namespace VerilogParser
{
    class VerilogElabItem;
    class VerilogElabItems;
    
    class VerilogElabModule
    {
        public:
            VerilogElabModule(const std::string& name_);
            virtual ~VerilogElabModule();

            // Get the name of the elaborated module
            inline const std::string& getName() const { return m_name_; }		

            // Item map accessors
            bool hasItem(const std::string& identifier_) const;
            const VerilogElabItem* getItem(const std::string& identifier_) const;
            const VerilogElabItemMap& getItemMap() const { return m_item_map_; }
            
            // Clone the elaborated module
            VerilogElabModule* clone() const;  
            
        // Protected copy constructor, use clone instead
        protected:
            VerilogElabModule(const VerilogElabModule* module_);
            
        private:
            // Copy a module, applying the appropriate parameter overwrites
            void copyModule(const VerilogElabModule* module_, const VerilogConns& parameter_values_);
            
        private:
            // Module name
            const std::string m_name_;
            // Elaborated items map (mapped by the identifier);
            VerilogElabItemMap m_item_map_;
    };
}
#endif
