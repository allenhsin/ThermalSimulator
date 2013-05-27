#ifndef VERILOG_PARSER_ELAB_INSTANCE_H
#define VERILOG_PARSER_ELAB_INSTANCE_H

#include <string>
#include "../VerilogMisc.h"
#include "ElabItem.h"

// A class representing a verilog module
namespace VerilogParser
{    
    class ElabModule;
    class VerilogScope;
    
    class ElabInstance : public ElabItem
    {
        public:
            ElabInstance(const std::string& identifier_, const VerilogScope* scope_, const ElabModule* module_);
            virtual ~ElabInstance();
            
            inline const ElabModule* getModule() const { return m_module_; }
            
            // Get a clone with flattened identifiers
            ElabItem* getFlattenedClone(const std::string& hier_, const std::string& hier_sep_) const;
            // Clone the item
            inline ElabItem* clone() const { return new ElabInstance(*this); }
            // Get a string representation of the item
            std::string toString() const;
            
        protected:
            // Protected copy constructor, use clone instead
            ElabInstance(const ElabInstance& instance_);
            
        private:
            // The elaborated module that the instance points to
            const ElabModule* m_module_;
            
    };
}
#endif
