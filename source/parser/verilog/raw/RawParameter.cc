
#include <iostream>

#include "RawParameter.h"

namespace VerilogParser
{
    using std::string;

    RawParameter::RawParameter(const string& identifier_, const RHExpression& value_)
        : RawItem(RAW_ITEM_PARAM, identifier_), m_value_(value_.clone())
    {}

    RawParameter::~RawParameter()
    {}
    
    ElabItems* RawParameter::elaborate(VerilogScope* scope_) const
    {
        ElabItems* elab_nets = new ElabItems();
        //TODO
        return elab_nets;
    }

    std::string RawParameter::toString() const
    {
        return RawItem::toString() + "=" + m_value_->toString();
    }
    
    RawItem* RawParameter::clone() const
    {
        return new RawParameter(*this);
    }
    
    RawParameter::RawParameter(const RawParameter& param_)
        : RawItem(param_),  m_value_(param_.m_value_->clone())
    {}
    
    
} // namespace VerilogParser
