
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "BinaryExpression.h"
#include "NumberExpression.h"
#include "../Number.h"

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
    
    string BinaryExpression::toString() const
    {
        return m_op_expr0_->toString() + m_bin_op_ + m_op_expr1_->toString();
    }    
    
} // namespace VerilogParser
