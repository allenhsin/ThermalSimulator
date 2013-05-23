
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "ConcatLHExpression.h"

namespace VerilogParser
{
    using std::string;

    ConcatLHExpression::ConcatLHExpression(const LHExpressions& exprs_)
        : Expression(LHCONCAT), LHExpression(LHCONCAT), m_exprs_()
    {
        for(LHExpressions::const_iterator it = exprs_.begin(); it != exprs_.end(); ++it)
            m_exprs_.push_back((*it)->clone()); 
    }
    
    ConcatLHExpression::~ConcatLHExpression()
    {
        clearPtrVector<LHExpression>(&m_exprs_);
    }
    
    ConcatLHExpression::ConcatLHExpression(const ConcatLHExpression& expr_)
        : Expression(LHCONCAT), LHExpression(LHCONCAT), m_exprs_()
    {
        for(LHExpressions::const_iterator it = expr_.m_exprs_.begin(); it != expr_.m_exprs_.end(); ++it)
            m_exprs_.push_back((*it)->clone()); 
    }
    
    string ConcatLHExpression::toString() const
    {
        std::string out = "{";
        for(LHExpressions::const_iterator it = m_exprs_.begin(); it != m_exprs_.end(); ++it)
            out += (*it)->toString() + ",";
        
        unsigned int pos = out.find_last_of(',');
        if (pos < out.length()) out[pos] = '}';
        else out += '}';

        return out;
    }    
    
} // namespace VerilogParser
