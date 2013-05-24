#ifndef VERILOG_PARSER_NUMBER_EXPRESSION_H
#define VERILOG_PARSER_NUMBER_EXPRESSION_H

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

    class NumberExpression : public RHExpression
    {
        public:
            NumberExpression(unsigned long num_);
            NumberExpression(const Number& num_);
            virtual ~NumberExpression();
            
            // Returns the expression as a printable string
            std::string toString() const;
            
        protected:
            // Accessors
            inline virtual const Number* accNumber() const { return &m_number_; }
            // Clone function
            inline NumberExpression* clone() const { return new NumberExpression(*this); }
            
        private:
            // Copy constructor
            NumberExpression(const NumberExpression& expr_);
            
        private:
            // Number in the expression
            const Number m_number_;
    };
}
#endif
