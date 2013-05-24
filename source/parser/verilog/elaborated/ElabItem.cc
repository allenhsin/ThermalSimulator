
#include "ElabItem.h"

namespace VerilogParser
{
    ElabItem::ElabItem(ElabItemType type_, const std::string& identifier_)
        : m_type_(type_), m_identifier_(identifier_)
    {}
    
    ElabItem::~ElabItem()
    {}
        
    ElabItem::ElabItem(const ElabItem& item_)
        : m_type_(item_.m_type_), m_identifier_(item_.m_identifier_)
    {}

    ElabItem* ElabItem::getFlattenedClone(const std::string& hier_, const std::string& /* hier_sep_ */) const
    {
        ElabItem* c = clone();
        c->m_identifier_ = hier_ + c->m_identifier_;        
        return c;
    }
    
} // namespace VerilogParser
