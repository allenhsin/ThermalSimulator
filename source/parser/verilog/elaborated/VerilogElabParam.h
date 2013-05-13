#ifndef THERMAL_PARSER_VERILOG_ELAB_PARAM_H
#define THERMAL_PARSER_VERILOG_ELAB_PARAM_H

#include <string>
#include <cstdlib>
#include "../VerilogMisc.h"
#include "VerilogElabItem.h"

namespace VerilogParser
{
    // An elaborated verilog parameter
    class VerilogElabParam : public VerilogElabItem
    {
        public:
            // Private constructor, use the static elaborate function instead
            VerilogElabParam(const std::string& identifier_, const std::string& value_);
            virtual ~VerilogElabParam();
            
        public:
            // Accessor functions
            inline const std::string& getStringValue() const { return m_value_; }
            inline unsigned int getUIntValue() const { return atoi(m_value_.c_str()); }
            inline double getDoubleValue() const { return atof(m_value_.c_str()); }
            
            // Clone
            VerilogElabItem* clone() const;            
            // Get string representation            
            std::string toString() const;
            
        protected:
            // Protected copy constructor, use clone instead
            VerilogElabParam(const VerilogElabParam& param_);
            
        private:
            // Port type
            const std::string& m_value_;
    };
}
#endif
