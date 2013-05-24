
#include "ElabItem.h"

namespace VerilogParser
{
    using std::string;

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
    
    string ElabItem::toString() const
    {
        switch (m_type_)
        {
            case ELAB_ITEM_PARAM: return "ELAB:PARAM>" + m_identifier_;
            case ELAB_ITEM_NET: return "ELAB:NET>" + m_identifier_;
            case ELAB_ITEM_INST: return "ELAB:INST>" + m_identifier_;
            case ELAB_ITEM_ASSIGN: return "ELAB:ASSIGN>" + m_identifier_;
            default: return "ELAB:BADTYPE>" + m_identifier_;
        }
    }
        
    IndexedElabItems::IndexedElabItems(int idx_offset_)
        : m_idx_offset_(idx_offset_), m_items_()
    {}
    
    IndexedElabItems::~IndexedElabItems()
    {}
    
} // namespace VerilogParser
