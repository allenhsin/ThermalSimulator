#ifndef VERILOG_PARSER_ELAB_PARAM_H
#define VERILOG_PARSER_ELAB_PARAM_H

#include <string>
#include <cstdlib>
#include "../VerilogMisc.h"
#include "../expressions/Expression.h"
#include "ElabItem.h"

namespace VerilogParser
{
    // An elaborated verilog parameter
    class ElabParam : public ElabItem
    {
        public:
            // Private constructor, use the static elaborate function instead
            ElabParam(const std::string& identifier_);
            virtual ~ElabParam();
            
        public:
            // Accessor functions
            //inline const RHExpression& getValue() const { return m_value_; }
            
            // Clone
            ElabItem* clone() const;            
            // Get string representation            
            std::string toString() const;
            
        protected:
            // Protected copy constructor, use clone instead
            ElabParam(const ElabParam& param_);
            
        private:
            // Value of the elaborated parameter
            //const RHExpression m_value_;
    };
}
#endif
