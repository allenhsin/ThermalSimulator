#ifndef VERILOG_PARSER_RAW_PARAMETER_H
#define VERILOG_PARSER_RAW_PARAMETER_H

#include <string>
#include <vector>
#include "RawItem.h"
#include "../expressions/Expression.h"

// A class representing a verilog module
namespace VerilogParser
{
    class VerilogScope;

    class RawParameter : public RawItem
    {
        public:
            RawParameter(const std::string& identifier_, const RHExpression& value_);
            virtual ~RawParameter();

        public:
            // Get the value
            const Expression& getValue() const { return *m_value_; }            
            
            // Create elaborated items
            IndexedElabItems* elaborate(VerilogScope* scope_) const;
            // Get printable string
            std::string toString() const;
            // Clone
            RawItem* clone() const;
        
        protected:
            // Protected copy constructor, use clone instead
            RawParameter(const RawParameter& param_);
            
        private:
            const RHExpression* m_value_;
    };
}
#endif
