#ifndef THERMAL_PARSER_VERILOG_ELAB_INSTANCE_H
#define THERMAL_PARSER_VERILOG_ELAB_INSTANCE_H

#include <string>
#include "../VerilogMisc.h"
#include "VerilogElabItem.h"

// A class representing a verilog module
namespace VerilogParser
{    
    class VerilogElabModule;
    
    class VerilogElabInstance : public VerilogElabItem
    {
        public:
            VerilogElabInstance(const std::string& identifier_, const VerilogElabConns& conns_,
                const VerilogElabModule* module_);
            virtual ~VerilogElabInstance();
            
            inline const VerilogElabConns& getConns() const { return m_conns_; }
            inline const VerilogElabModule* getModule() const { return m_module_; }
            
            // Get a clone with flattened identifiers
            VerilogElabItem* getFlattenedClone(const std::string& hier_, const std::string& hier_sep_) const;
            // Clone the item
            VerilogElabItem* clone() const;
            // Get a string representation of the item
            std::string toString() const;            
            
        protected:
            // Protected copy constructor, use clone instead
            VerilogElabInstance(const VerilogElabInstance& instance_);
            
        private:
            // Instance connections
            const VerilogElabConns m_conns_;
            // The elaborated module that the instance points to
            const VerilogElabModule* m_module_;
            
    };
}
#endif
