#ifndef VERILOG_PARSER_CONCATLH_EXPRESSION_H
#define VERILOG_PARSER_CONCATLH_EXPRESSION_H

#include <string>
#include <vector>

#include "Expression.h"
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class Number;

    class ConcatLHExpression : public LHExpression
    {
        public:
            ConcatLHExpression(const LHExpressions& exprs_);
            virtual ~ConcatLHExpression();
            
            // Returns the expression as a printable string
            std::string toString() const;
            
        protected:
            // Accessors
            inline virtual const LHExpressions* accLHConcat() const { return &m_exprs_; }
            // Clone function
            inline ConcatLHExpression* clone() const { return new ConcatLHExpression(*this); }
            
        private:
            // Copy constructor
            ConcatLHExpression(const ConcatLHExpression& expr_);
            
        private:
            // The LH expressions
            LHExpressions m_exprs_;
    };
}
#endif
