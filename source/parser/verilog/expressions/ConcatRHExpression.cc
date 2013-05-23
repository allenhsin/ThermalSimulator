
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "ConcatRHExpression.h"

namespace VerilogParser
{
    using std::string;

    ConcatRHExpression::ConcatRHExpression(const RHExpressions& exprs_)
        : Expression(RHCONCAT), RHExpression(RHCONCAT), m_exprs_()
    {
        for(RHExpressions::const_iterator it = exprs_.begin(); it != exprs_.end(); ++it)
            m_exprs_.push_back((*it)->clone()); 
    }
    
    ConcatRHExpression::~ConcatRHExpression()
    {
        clearPtrVector<RHExpression>(&m_exprs_);
    }
    
    ConcatRHExpression::ConcatRHExpression(const ConcatRHExpression& expr_)
        : Expression(expr_), RHExpression(expr_), m_exprs_()
    {
        for(RHExpressions::const_iterator it = expr_.m_exprs_.begin(); it != expr_.m_exprs_.end(); ++it)
            m_exprs_.push_back((*it)->clone()); 
    }
    
    string ConcatRHExpression::toString() const
    {
        std::string out = "{";
        for(RHExpressions::const_iterator it = m_exprs_.begin(); it != m_exprs_.end(); ++it)
            out += (*it)->toString() + ",";
        
        unsigned int pos = out.find_last_of(',');
        if (pos < out.length()) out[pos] = '}';
        else out += '}';

        return out;
    }    
    
} // namespace VerilogParser
