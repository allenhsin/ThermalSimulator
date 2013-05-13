#ifndef THERMAL_PARSER_VERILOG_ELAB_PARAM_H
#define THERMAL_PARSER_VERILOG_ELAB_PARAM_H

#include <string>
#include <cstdlib>
#include "../VerilogMisc.h"
#include "../VerilogExpression.h"
#include "VerilogElabItem.h"

namespace VerilogParser
{
    // An elaborated verilog parameter
    class VerilogElabParam : public VerilogElabItem
    {
        public:
            // Private constructor, use the static elaborate function instead
            VerilogElabParam(const std::string& identifier_, const VerilogExpression& value_);
            virtual ~VerilogElabParam();
            
        public:
            // Accessor functions
            inline const VerilogExpression& getValue() const { return m_value_; }
            
            // Clone
            VerilogElabItem* clone() const;            
            // Get string representation            
            std::string toString() const;
            
        protected:
            // Protected copy constructor, use clone instead
            VerilogElabParam(const VerilogElabParam& param_);
            
        private:
            // Value of the elaborated parameter
            const VerilogExpression m_value_;
    };
}
#endif
