#ifndef VERILOG_PARSER_STRING_EXPRESSION_H
#define VERILOG_PARSER_STRING_EXPRESSION_H

#include <string>
#include <vector>

#include "Expression.h"
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class Number;

    class StringExpression : public RHExpression
    {
        public:
            StringExpression(const std::string& str_);
            virtual ~StringExpression();
            
            // Returns the expression as a printable string
            std::string toString() const;
            
        protected:
            // Accessors
            inline virtual const std::string* accString() const { return &m_str_; }
            // Clone function
            inline StringExpression* clone() const { return new StringExpression(*this); }
            
        private:
            // Copy constructor
            StringExpression(const StringExpression& expr_);
            
        private:
            // Number in the expression
            const std::string m_str_;
    };
}
#endif
