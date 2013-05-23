
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "IdentifierExpression.h"
#include "NumberExpression.h"
#include "../Number.h"

namespace VerilogParser
{
    using std::string;

    IdentifierExpression::IdentifierExpression(const string& identifier_, const BitRange& range_)
        : Expression(IDENTIFIER), LHExpression(IDENTIFIER), RHExpression(IDENTIFIER), 
            m_identifier_(identifier_), m_range_(range_), m_has_range_(true)
    {}

    IdentifierExpression::IdentifierExpression(const string& identifier_)
        : Expression(IDENTIFIER),LHExpression(IDENTIFIER), RHExpression(IDENTIFIER), m_identifier_(identifier_), 
            m_range_(NumberExpression(0), NumberExpression(0)), m_has_range_(false)
    {}

    IdentifierExpression::IdentifierExpression(const IdentifierExpression& expr_)
        : Expression(IDENTIFIER), LHExpression(IDENTIFIER), RHExpression(IDENTIFIER), 
            m_identifier_(expr_.m_identifier_), m_range_(expr_.m_range_), m_has_range_(expr_.m_has_range_)
    {}
            
    IdentifierExpression::~IdentifierExpression()
    {}
    
    string IdentifierExpression::toString() const
    {
        if (m_has_range_)
            return m_identifier_ + m_range_.toString();
        return m_identifier_;
    }
    
    
} // namespace VerilogParser
