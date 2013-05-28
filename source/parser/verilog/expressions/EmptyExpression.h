#ifndef VERILOG_PARSER_EMPTY_EXPRESSION_H
#define VERILOG_PARSER_EMPTY_EXPRESSION_H

#include <string>
#include <vector>
#include <iostream>

#include "Expression.h"
#include "../Number.h"
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class Number;

    class EmptyExpression : public RHExpression
    {
        public:
            EmptyExpression();
            virtual ~EmptyExpression();
            
            // Return the expression as usable bit targets
            BitVector* elaborate(VerilogScope* scope_) const;
            // Returns the expression as a printable string
            std::string toString() const;
            // Clone function
            inline EmptyExpression* clone() const { return new EmptyExpression(*this); }
            
        private:
            // Copy constructor
            EmptyExpression(const EmptyExpression& expr_);

    };
}
#endif
