
#include "VerilogElabItem.h"

namespace VerilogParser
{
    VerilogElabItem::VerilogElabItem(VerilogItemType type_, const std::string& identifier_)
        : m_type_(type_), m_identifier_(identifier_)
    {}
    
    VerilogElabItem::~VerilogElabItem()
    {}
        
    VerilogElabItem::VerilogElabItem(const VerilogElabItem& item_)
        : m_type_(item_.m_type_), m_identifier_(item_.m_identifier_)
    {}

    VerilogElabItem* VerilogElabItem::getFlattenedClone(const std::string& hier_, const std::string& /* hier_sep_ */) const
    {
        VerilogElabItem* c = clone();
        c->m_identifier_ = hier_ + c->m_identifier_;        
        return c;
    }
    
} // namespace VerilogParser
