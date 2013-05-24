
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "expressions/Expression.h"
#include "VerilogMisc.h"

namespace VerilogParser
{
    using namespace std;
    using namespace LibUtil;

    // VerilogElabRange::VerilogElabRange(int high_, int low_)
        // : high(high_), low(low_)
    // {}
    
    // VerilogElabRange::~VerilogElabRange()
    // {}
    
    BitRange::BitRange(const Expression& high_, const Expression& low_)
        : high(high_.clone()), low(low_.clone())
    {}
    
    BitRange::BitRange(const BitRange& range_)
        : high(range_.high->clone()), low(range_.high->clone())
    {}
    
    BitRange::~BitRange()
    {
        delete high;
        delete low;
    }
    
    std::string BitRange::toString() const
    {
        return "[" + high->toString() + ":" + low->toString() + "]";
    }
    
    SetValue::SetValue(const std::string& identifier_, const Expression& value_)
        : identifier(identifier_), value(value_.clone())
    {}
    
    SetValue::~SetValue()
    {}
    
    VerilogException::VerilogException(const std::string& message_)
        : exception(), m_message_(message_)
    {}

    VerilogException::~VerilogException() throw()
    {}
    
} // namespace VerilogParser
