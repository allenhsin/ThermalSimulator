
#include <iostream>

#include "ConcatRHExpression.h"
#include "../BitVector.h"

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
        : Expression(RHCONCAT), RHExpression(RHCONCAT), m_exprs_()
    {
        for(RHExpressions::const_iterator it = expr_.m_exprs_.begin(); it != expr_.m_exprs_.end(); ++it)
            m_exprs_.push_back((*it)->clone()); 
    }
    
    BitVector* ConcatRHExpression::elaborate(VerilogScope* scope_) const
    {
        BitVector* bt_out = new BitVector();
        // Remember to start from the back
        RHExpressions::const_reverse_iterator it;
        for (it = m_exprs_.rbegin(); it != m_exprs_.rend(); ++it)
        {
            BitVector* bt_cur = (*it)->elaborate(scope_);
            bt_out->append(*bt_cur);

            delete bt_cur;
        }
        return bt_out;
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
