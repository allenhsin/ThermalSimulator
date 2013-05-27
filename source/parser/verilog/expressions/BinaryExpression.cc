
#include <cassert>
#include <stdio.h>
#include <cstdlib>
#include <iostream>

#include "BinaryExpression.h"
#include "NumberExpression.h"
#include "../Number.h"
#include "../BitVector.h"

namespace VerilogParser
{
    using std::string;

    BinaryExpression::BinaryExpression(const RHExpression& op_expr0_, const BinOperator& bin_op_, const RHExpression& op_expr1_)
        : Expression(BINARY), RHExpression(BINARY), m_op_expr0_(op_expr0_.clone()), m_op_expr1_(op_expr1_.clone()), m_bin_op_(bin_op_)
    {}
    
    BinaryExpression::~BinaryExpression()
    {
        delete m_op_expr0_;
        delete m_op_expr1_;
    }
    
    BinaryExpression::BinaryExpression(const BinaryExpression& expr_)
        : Expression(BINARY), RHExpression(BINARY), m_op_expr0_(expr_.m_op_expr0_->clone()), m_op_expr1_(expr_.m_op_expr1_->clone()), m_bin_op_(expr_.m_bin_op_)
    {}
    
    BitVector* BinaryExpression::elaborate(VerilogScope* scope_) const
    {
        BitVector* bt_0 = m_op_expr0_->elaborate(scope_);
        BitVector* bt_1 = m_op_expr1_->elaborate(scope_);
        
        if (!(bt_0->isConst() && bt_1->isConst()))
            throw VerilogException("Operations on non-constants are not supported: " + toString());
        
        // Figure out the maximum width of the two operands
        unsigned long width = std::max(bt_0->width(), bt_1->width());
        
        BitVector* bt_out;        
        if (m_bin_op_ == "+")
            bt_out = BitVector::makeBitVector(bt_0->getConstNum() + bt_1->getConstNum(), width);
        else if (m_bin_op_ == "-")
            bt_out = BitVector::makeBitVector(bt_0->getConstNum() - bt_1->getConstNum(), width);
        else if (m_bin_op_ == "*")
            bt_out = BitVector::makeBitVector(bt_0->getConstNum() * bt_1->getConstNum(), width);
        else if (m_bin_op_ == "/")
            bt_out = BitVector::makeBitVector(bt_0->getConstNum() / bt_1->getConstNum(), width);
        else if (m_bin_op_ == "%")
            bt_out = BitVector::makeBitVector(bt_0->getConstNum() % bt_1->getConstNum(), width);
        else
            throw new VerilogException("Bad binary operator: " + m_bin_op_);
        
        delete bt_0;
        delete bt_1;        
        return bt_out;
    }
    
    string BinaryExpression::toString() const
    {
        return "(" + m_op_expr0_->toString() + m_bin_op_ + m_op_expr1_->toString() + ")";
    }
    
} // namespace VerilogParser
