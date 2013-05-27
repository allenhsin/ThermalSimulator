
#include <cassert>
#include <stdio.h>

#include "ElabParam.h"
#include "../BitVector.h"

namespace VerilogParser
{
    using std::string;

    ElabParam::ElabParam(const std::string& identifier_, const VerilogScope* scope_, const BitVector* value_)
        : ElabItem(ELAB_ITEM_PARAM, identifier_, scope_), m_value_(value_)
    {}

    ElabParam::~ElabParam()
    {}
    
    std::string ElabParam::toString() const
    {
        return ElabItem::toString() + "=" + m_value_->toString();
    }
    
    ElabItem* ElabParam::clone() const
    {
        return new ElabParam(*this);
    }
    
    ElabParam::ElabParam(const ElabParam& param_)
        : ElabItem(param_), m_value_(param_.m_value_)
    {}

    
} // namespace VerilogParser
