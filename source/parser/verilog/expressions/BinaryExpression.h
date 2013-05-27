#ifndef VERILOG_PARSER_BINARY_EXPRESSION_H
#define VERILOG_PARSER_BINARY_EXPRESSION_H

#include <string>
#include <vector>

#include "Expression.h"
#include "../VerilogMisc.h"

// A class representing a verilog module
namespace VerilogParser
{
    class Number;

    class BinaryExpression : public RHExpression
    {
        public:
            BinaryExpression(const RHExpression& op_expr0_, const BinOperator& bin_op_, const RHExpression& op_expr1_);
            virtual ~BinaryExpression();
            
            // Return the expression as usable bit targets
            BitVector* elaborate(VerilogScope* scope_) const;
            // Returns the expression as a printable string
            std::string toString() const;
            // Clone function
            inline BinaryExpression* clone() const { return new BinaryExpression(*this); }
            
        protected:
            // Accessors
            inline virtual const RHExpression* accOpExpr0() const { return m_op_expr0_; }
            inline virtual const RHExpression* accOpExpr1() const { return m_op_expr0_; }
            inline virtual const BinOperator* accBinOp() const { return &m_bin_op_; }
            
        private:
            // Copy constructor
            BinaryExpression(const BinaryExpression& expr_);
            
        private:
            // The operands and the binary operator
            const RHExpression* m_op_expr0_;
            const RHExpression* m_op_expr1_;
            const BinOperator m_bin_op_;
    };
}
#endif
