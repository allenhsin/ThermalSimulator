#ifndef VERILOG_PARSER_RAW_INSTANCE_H
#define VERILOG_PARSER_RAW_INSTANCE_H

#include <string>
#include <vector>
#include "RawItem.h"
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class ElabModule;
    
    class RawInstance : public RawItem
    {
    
        public:
            RawInstance(const std::string& identifier_, const SetValues& conns_);
            virtual ~RawInstance();
            
            inline const SetValues* getConns() const { return m_conns_; }            
            inline const std::string& getModuleName() const { return m_module_name_; }
            
            // Set the name of the module that this instance instantiates
            inline void setModuleName(const std::string& module_name_ ) { m_module_name_ = module_name_; }
            // Set parameter values
            void setParameterValues(const SetValues& conns_);
            
            // Elaborate the item in the given verilog scope for the given module
            void elaborate(ElabModule* module_, VerilogScope* scope_) const;
            // Get printable string
            std::string toString() const;            
            // Clone the item
            inline RawInstance* clone() const { return new RawInstance(*this); }
            
        protected:
            // Protected copy constructor, use clone instead
            RawInstance(const RawInstance& instance_);
            
        private:
            // Instance connections
            SetValues* m_conns_;
            // Instance parameter values
            SetValues* m_params_;
            // Instance module name
            std::string m_module_name_;
    };
}
#endif
