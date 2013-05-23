
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "StringExpression.h"
#include "../Number.h"

namespace VerilogParser
{
    using std::string;

    StringExpression::StringExpression(const string& str_)
        : Expression(STRING), RHExpression(STRING), m_str_(str_)
    {}

    StringExpression::StringExpression(const StringExpression& expr_)
        : Expression(STRING), RHExpression(STRING), m_str_(expr_.m_str_)
    {}
    
    StringExpression::~StringExpression()
    {}
    
    string StringExpression::toString() const
    {
        return m_str_;
    }
    
    
} // namespace VerilogParser
