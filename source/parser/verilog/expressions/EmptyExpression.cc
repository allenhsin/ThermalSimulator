
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "EmptyExpression.h"
#include "../Number.h"
#include "../BitVector.h"

namespace VerilogParser
{
    using std::string;

    EmptyExpression::EmptyExpression()
        : Expression(EMPTY), RHExpression(EMPTY)
    {}
    
    EmptyExpression::EmptyExpression(const EmptyExpression& /* expr_ */)
        : Expression(EMPTY), RHExpression(EMPTY)
    {}
    
    EmptyExpression::~EmptyExpression()
    {}
    
    BitVector* EmptyExpression::elaborate(VerilogScope* /* scope_ */) const
    {        
        return new BitVector();
    }
    
    string EmptyExpression::toString() const
    {
        return "{}";
    }
    
    
} // namespace VerilogParser
