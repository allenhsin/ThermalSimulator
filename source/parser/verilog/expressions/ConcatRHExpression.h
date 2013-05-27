#ifndef VERILOG_PARSER_CONCATRH_EXPRESSION_H
#define VERILOG_PARSER_CONCATRH_EXPRESSION_H

#include <string>
#include <vector>

#include "Expression.h"
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class Number;

    class ConcatRHExpression : public RHExpression
    {
        public:
            ConcatRHExpression(const RHExpressions& exprs_);
            virtual ~ConcatRHExpression();
            
            // Return the expression as usable bit targets
            BitVector* elaborate(VerilogScope* scope_) const;
            // Returns the expression as a printable string
            std::string toString() const;
            
        protected:
            // Accessors
            inline virtual const RHExpressions* accRHConcat() const { return &m_exprs_; }
            // Clone function
            inline ConcatRHExpression* clone() const { return new ConcatRHExpression(*this); }
            
        private:
            // Copy constructor
            ConcatRHExpression(const ConcatRHExpression& expr_);
            
        private:
            // The RH expressions
            RHExpressions m_exprs_;
    };
}
#endif
