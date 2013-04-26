#ifndef THERMAL_PARSER_VERILOG_MODULE_H
#define THERMAL_PARSER_VERILOG_MODULE_H

#include <string>
#include <vector>
#include <map>
#include "VerilogMisc.h"

// A class representing a verilog module
namespace Thermal
{
    class VerilogNet;
    class VerilogInstance;
	class VerilogScope;
    
    class VerilogModule
    {
        public:
            VerilogModule(const std::string& name_, const VerilogItems& items_);
            virtual ~VerilogModule();

			// Elaborates the verilog module (and all items in the module)
			void elaborate(VerilogScope* scope_); 
     
            // Has a verilog module item
            bool hasItem(const std::string& identifier_) const;
            // Get an item in the module
            const VerilogItem* getItem(const std::string& identifier_) const;

            // Get the item map
            const VerilogItemMap* getItemMap() const { return m_item_map_; }
            // Get the name of the module
            inline const std::string& getName() const { return m_name_; }
		
        private:
            // Initialize the the module based on the module items
            void initializeItems(const VerilogItems& items_);
            
        private:
            // Module name
            const std::string m_name_;
			// Marks whether this module has been elaborated or not
			bool m_elaborated_;
            // Initialized items
            VerilogItems* m_items_;			
            // Item map (mapped by the identifier);
            VerilogItemMap* m_item_map_;
    };
}
#endif
